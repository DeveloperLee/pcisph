
#include "Particle.h"
#include "visualization/geometry/SDF.h"
#include "visualization/geometry/Voxelizer.h"
#include "visualization/mesh/Mesh.h"
#include "visualization/grid/Grid.h"
#include "utils/Math.h"

#include <Eigen/Geometry>

namespace cs224 {

ParticleGenerator::Boundary ParticleGenerator::generateFromBoundaryBox(const Box3f &box_, float particleRadius, bool innerNormal) {

    Boundary ret;

    float normalFlag = innerNormal ? 1.f : -1.f;

    Box3f box(box_);
    box.min -= Vector3f(particleRadius * normalFlag);
    box.max += Vector3f(particleRadius * normalFlag);

    Vector3f origin = box.min;
    Vector3f extents = box.extents();

    // compute particle num on each dimension
    int nx = std::ceil(extents.x() / (2.f * particleRadius));
    int ny = std::ceil(extents.y() / (2.f * particleRadius));
    int nz = std::ceil(extents.z() / (2.f * particleRadius));

    Vector3f diff = extents.cwiseQuotient(Vector3f(nx, ny, nz));

    // use a lambda function to add particles
    auto addParticle = [&ret, &origin, &diff, &normalFlag] (int x, int y, int z, const Vector3f &n) {
        ret.positions.emplace_back(origin + Vector3f(x, y, z).cwiseProduct(diff));
        ret.normals.emplace_back(n * normalFlag);
    };

    // xy
    for (int x = 1; x < nx; ++ x) {
        for (int y = 1; y < ny; ++ y) {
            addParticle(x, y,  0, Vector3f(0.f, 0.f,  1.f));
            addParticle(x, y, nz, Vector3f(0.f, 0.f, -1.f));
        }
    }

    // xz
    for (int x = 1; x < nx; ++ x) {
        for (int z = 1; z < nz; ++ z) {
            addParticle(x,  0, z, Vector3f(0.f,  1.f, 0.f));
            addParticle(x, ny, z, Vector3f(0.f, -1.f, 0.f));
        }
    }

    // yz
    for (int y = 1; y < ny; ++ y) {
        for (int z = 1; z < nz; ++ z) {
            addParticle( 0, y, z, Vector3f( 1.f, 0.f, 0.f));
            addParticle(nx, y, z, Vector3f(-1.f, 0.f, 0.f));
        }
    }

    // x borders
    for (int x = 1; x < nx; ++ x) {
        addParticle(x ,  0,  0, Vector3f( 0.f,  1.f,  1.f).normalized());
        addParticle(x , ny,  0, Vector3f( 0.f, -1.f,  1.f).normalized());
        addParticle(x ,  0, nz, Vector3f( 0.f,  1.f, -1.f).normalized());
        addParticle(x , ny, nz, Vector3f( 0.f, -1.f, -1.f).normalized());
    }

    // y borders
    for (int y = 1; y < ny; ++ y) {
        addParticle( 0, y ,  0, Vector3f( 1.f, 0.f,  1.f).normalized());
        addParticle(nx, y ,  0, Vector3f(-1.f, 0.f,  1.f).normalized());
        addParticle( 0, y , nz, Vector3f( 1.f, 0.f, -1.f).normalized());
        addParticle(nx, y , nz, Vector3f(-1.f, 0.f, -1.f).normalized());
    }

    // z borders
    for (int z = 1; z < nz; ++ z) {
        addParticle( 0,  0, z, Vector3f( 1.f,  1.f,  0.f).normalized());
        addParticle(nx,  0, z, Vector3f(-1.f,  1.f,  0.f).normalized());
        addParticle( 0, ny, z, Vector3f( 1.f, -1.f,  0.f).normalized());
        addParticle(nx, ny, z, Vector3f(-1.f, -1.f,  0.f).normalized());
    }

    // corners
    for (int i = 0; i < 8; ++ i) {
        int x = (i     ) & 1;
        int y = (i >> 1) & 1;
        int z = (i >> 2) & 1;
        addParticle(x ? 0 : nx, y ? 0 : ny, z ? 0 : nz, Vector3f(x ? 1.f : -1.f, y ? 1.f : -1.f, z ? 1.f : -1.f).normalized());
    }

    return ret;
}

ParticleGenerator::Boundary ParticleGenerator::generateFromBoundaryMesh(const Mesh &mesh, float particleRadius, int cells) {

    float density = 1.f / (PI * pow2(particleRadius));

    // compute bounds of mesh and expand by 10%
    Box3f bounds = mesh.bound();
    bounds = bounds.expanded(bounds.extents());

    // compute cell and grid size for signed distance field
    float cellSize = bounds.extents()[bounds.majorAxis()] / cells;

    Vector3i size(
        int(std::ceil(bounds.extents().x() / cellSize)),
        int(std::ceil(bounds.extents().y() / cellSize)),
        int(std::ceil(bounds.extents().z() / cellSize))
    );

    VoxelGrid<float> sdf(size);
    sdf.setOrigin(bounds.min);
    sdf.setCellSize(cellSize);

    // build signed distance field
    SDF::build(mesh, sdf);

    // generate initial point distribution
    Boundary ret;
    float totalArea = 0.f;

    for (int i = 0; i < mesh.triangles().cols(); ++ i) {
        const Vector3f &p0 = mesh.vertices().col(mesh.triangles()(0, i));
        const Vector3f &p1 = mesh.vertices().col(mesh.triangles()(1, i));
        const Vector3f &p2 = mesh.vertices().col(mesh.triangles()(2, i));

        Vector3f v1 = p1 - p0;
        Vector3f v2 = p2 - p0;

        float area = 0.5f * std::abs(v1.cross(v2).norm());
        totalArea += area;

        int numi = int(std::floor(density * area));

        auto samplePoint = [&]() {
            float s = randomFloat();
            float t = randomFloat();
            float ssq = std::sqrt(s);
            ret.positions.emplace_back(p0 + v1 * t * ssq + v2 * (1.f - ssq));
        };

        // sample particles on the triangle randomly
        for (int num = 0; num < numi; ++ num) {
            samplePoint();
        }
    }

    // choose radius to support roughly 10 neighbour particles
    float radius = std::sqrt(totalArea / ret.positions.size() * 10.f / PI);
    float radius_sq = pow2(radius);

    Grid grid;
    grid.init(bounds, bounds.extents().maxCoeff() / 128.f);

    // keep doing 10 times to smooth particle positions
    for (int iteration = 0; iteration < 10; ++ iteration) {
        int count = 0;
        std::vector<Vector3f> velocities(ret.positions.size(), Vector3f());

        grid.update(ret.positions, [&](size_t i, size_t j) {
            std::swap(ret.positions[i], ret.positions[j]);
        });

        // relax positions
        for (size_t i = 0; i < ret.positions.size(); ++ i) {
            grid.lookup(ret.positions[i], radius, [&](size_t j) {
                if (i == j) return true;
                Vector3f r = ret.positions[j] - ret.positions[i];
                float rsq = r.squaredNorm();

                if (rsq < radius_sq) {
                    r *= (1.f / std::sqrt(rsq));
                    float weight = 0.002f * pow3(1.f - rsq / radius_sq);
                    velocities[i] -= weight * r;
                    count += 1;
                }

                return true;
            });

            ret.positions[i] += velocities[i];
        }

        // reproject to surface
        for (size_t i = 0; i < ret.positions.size(); ++ i) {
            Vector3f p = sdf.toVoxelSpace(ret.positions[i]);
            Vector3f n = sdf.gradient(p).normalized();
            ret.positions[i] -= sdf.trilinear(p) * n;
        }
    }

    // compute normals
    ret.normals.resize(ret.positions.size());
    for (size_t i = 0; i < ret.positions.size(); ++ i) {
        ret.normals[i] = sdf.gradient(sdf.toVoxelSpace(ret.positions[i])).normalized();
    }

    return ret;
}

ParticleGenerator::Volume ParticleGenerator::generateFromVolumeBox(const Box3f &box, float particleRadius) {

    Volume ret;
    Vector3f extents = box.extents();

    // compute particle num on each dimension
    int nx = std::ceil(extents.x() / (2.f * particleRadius));
    int ny = std::ceil(extents.y() / (2.f * particleRadius));
    int nz = std::ceil(extents.z() / (2.f * particleRadius));

    Vector3f start = box.min + Vector3f(particleRadius);
    Vector3f diff = extents.cwiseQuotient(Vector3f(nx, ny, nz));

    for (int x = 0; x < nx; ++ x) {
        for (int y = 0; y < ny; ++ y) {
            for (int z = 0; z < nz; ++ z) {
                ret.positions.emplace_back(start + Vector3f(x, y, z).cwiseProduct(diff));
            }
        }
    }

    return ret;
}

ParticleGenerator::Volume ParticleGenerator::generateFromVolumeSphere(const Vector3f &center, float radius, float particleRadius) {

    Volume ret;
    float diameter = 2.f * particleRadius;

    Vector3i min(
        int(std::ceil((center.x() - radius) / diameter)),
        int(std::ceil((center.y() - radius) / diameter)),
        int(std::ceil((center.z() - radius) / diameter))
    );

    Vector3i max(
        int(std::floor((center.x() + radius) / diameter)),
        int(std::floor((center.y() + radius) / diameter)),
        int(std::floor((center.z() + radius) / diameter))
    );

    float radiusSq = pow2(radius);

    for (int z = min.z(); z <= max.z(); ++z) {
        for (int y = min.y(); y <= max.y(); ++y) {
            for (int x = min.x(); x <= max.x(); ++x) {
                Vector3f pos(x * diameter, y * diameter, z * diameter);
                if ((pos - center).squaredNorm() <= radiusSq) ret.positions.emplace_back(pos);
            }
        }
    }

    return ret;
}

ParticleGenerator::Volume ParticleGenerator::generateFromVolumeMesh(const Mesh &mesh, float particleRadius) {

    Volume ret;

    Voxelizer::voxelize(mesh, 2.f * particleRadius, ret.positions);

    return ret;
}

} // namespace cs224

