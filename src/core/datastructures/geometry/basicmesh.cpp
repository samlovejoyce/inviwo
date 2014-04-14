#include <inviwo/core/datastructures/geometry/basicmesh.h>
#include <inviwo/core/datastructures/buffer/bufferramprecision.h>

#ifdef WIN32
#define _USE_MATH_DEFINES
#include <math.h>
#endif

namespace inviwo {
BasicMesh::BasicMesh()
    : Mesh() {
        
    vertices_ = new Position3dBuffer();
    addAttribute(vertices_);
    
    texCoords_ = new TexCoord3dBuffer();
    addAttribute(texCoords_);
    
    colors_ = new ColorBuffer();
    addAttribute(colors_);
    
    normals_ = new NormalBuffer();
    addAttribute(normals_);
        
}

BasicMesh::~BasicMesh() {
    deinitialize();
}
 
    
void BasicMesh::addVertex(vec3 pos, vec3 normal, vec3 texCoord, vec4 color){
    vertices_->getEditableRepresentation<Position3dBufferRAM>()->add(pos);
    normals_->getEditableRepresentation<NormalBufferRAM>()->add(normal);
    texCoords_->getEditableRepresentation<TexCoord3dBufferRAM>()->add(texCoord);
    colors_->getEditableRepresentation<ColorBufferRAM>()->add(color);
}

IndexBufferRAM* BasicMesh::addIndexBuffer(RenderType rt, ConnectivityType ct) {
    IndexBuffer* indices_ = new IndexBuffer();
    addIndicies(Mesh::AttributesInfo(rt,ct), indices_);
    return indices_->getEditableRepresentation<IndexBufferRAM>();
}
    
void BasicMesh::append(const BasicMesh* mesh) {
    const Position3dBufferRAM* pos = mesh->getVertices()->getRepresentation<Position3dBufferRAM>();
    const NormalBufferRAM* norm = mesh->getNormals()->getRepresentation<NormalBufferRAM>();
    const TexCoord3dBufferRAM* tex = mesh->getTexCoords()->getRepresentation<TexCoord3dBufferRAM>();
    const ColorBufferRAM* col = mesh->getColors()->getRepresentation<ColorBufferRAM>();
    
    size_t size = pos->size();
    
    vertices_->getEditableRepresentation<Position3dBufferRAM>()->append(pos->getDataContainer());
    normals_->getEditableRepresentation<NormalBufferRAM>()->append(norm->getDataContainer());
    texCoords_->getEditableRepresentation<TexCoord3dBufferRAM>()->append(tex->getDataContainer());
    colors_->getEditableRepresentation<ColorBufferRAM>()->append(col->getDataContainer());
    
    for (size_t i = 0; i < mesh->indexAttributes_.size(); ++i) {
        std::pair<AttributesInfo, IndexBuffer*> buffer = mesh->indexAttributes_[i];
        
        IndexBufferRAM* ind = addIndexBuffer(buffer.first.rt, buffer.first.ct);
 
        const std::vector<unsigned int>* newinds =
            buffer.second->getRepresentation<IndexBufferRAM>()->getDataContainer();
        
        //transform(newinds->begin(), newinds->end(), ind->getDataContainer()->end(),
        //          bind2nd(std::plus<unsigned int>(), size));
        
        for (std::vector<unsigned int>::const_iterator it = newinds->begin(); it!=newinds->end(); ++it) {
            ind->add(static_cast<const unsigned int>(*it + size));
        }
    }
}

const Position3dBuffer* BasicMesh::getVertices() const {
    return vertices_;
}
const TexCoord3dBuffer* BasicMesh::getTexCoords() const {
    return texCoords_;
}
const ColorBuffer* BasicMesh::getColors() const {
    return colors_;
}
const NormalBuffer* BasicMesh::getNormals() const {
    return normals_;
}
    
    
vec3 BasicMesh::orthvec(const vec3& vec){
    vec3 u(1.0f, 0.0f, 0.0f);
    vec3 n = glm::normalize(vec);
    float p = glm::dot(u, n);
    if (abs(p) != 1.0f) {
        return glm::normalize(u - p*n);
    }else{
        return vec3(0.0f,1.0f,0.0f);
    }
}

vec3 BasicMesh::calcnormal(const vec3& r, const vec3& p) {
    return glm::normalize(r + glm::dot(r, r - p) / glm::dot(p - r, p - r)* (p-r));
}

BasicMesh* BasicMesh::disk(const vec3& center,
                           const vec3& normal,
                           const vec4& color,
                           const float& radius,
                           const size_t& segments) {

    BasicMesh* mesh = new BasicMesh();
    IndexBufferRAM* inds = mesh->addIndexBuffer(TRIANGLES, NONE);
    vec3 orth = orthvec(normal);
    
    mesh->addVertex(center, normal, vec3(0.5f,0.5f,0.0f), color);
    
    vec3 tc = vec3(0.5f,0.5f,0.0f);
    vec3 tn = vec3(0.0f,0.0f,1.0f);
    vec3 to = vec3(0.5f,0.0f,0.0f);
    vec3 p;
    vec3 t;
    float angle = M_2_PI / segments;
    for (size_t i = 0; i < segments; ++i) {
        p = center + radius * glm::rotate(orth, static_cast<float>(i) * angle, normal);
        t = tc + glm::rotate(to,static_cast<float>(i) * angle, tn);
        mesh->addVertex(p, normal, t, color);
        inds->add(0);
        inds->add(static_cast<const unsigned int>(1+i));
        inds->add(static_cast<const unsigned int>(1+( (i+1) % segments)));
    }
    return mesh;
}
    
BasicMesh* BasicMesh::cone(const vec3& start,
                           const vec3& stop,
                           const vec4& color,
                           const float& radius,
                           const size_t& segments) {
    vec3 tc = vec3(0.5f, 0.5f, 0.0f);
    vec3 tn = vec3(0.0f, 0.0f, 1.0f);
    vec3 to = vec3(0.5f, 0.0f, 0.0f);

    BasicMesh* mesh = new BasicMesh();
    IndexBufferRAM* inds = mesh->addIndexBuffer(TRIANGLES, NONE);
    vec3 normal = glm::normalize(stop-start);
    vec3 orth = orthvec(normal);
    float angle = M_2_PI / segments;
    float j;
    for (size_t i = 0; i < segments; ++i) {
        j = static_cast<float>(i);
        mesh->addVertex(stop, 
                        calcnormal(glm::rotate(orth, (j + 0.5f) * angle, normal), stop - start), 
                        tc, 
                        color);

        mesh->addVertex(start + radius*glm::rotate(orth, j * angle, normal),
                        calcnormal(glm::rotate(orth, j * angle, normal), stop - start),
                        tc + to*glm::rotate(orth, j * angle, tn),
                        color);

        mesh->addVertex(start + radius*glm::rotate(orth, (j+1.0f) * angle, normal),
                        calcnormal(glm::rotate(orth, (j+1.0f) * angle, normal), stop - start),
                        tc + to*glm::rotate(orth, (j+1) * angle, tn),
                        color);

        inds->add(i * 3 + 0);
        inds->add(i * 3 + 1);
        inds->add(i * 3 + 2);
    }

    return mesh;
}

    
    
} // namespace

