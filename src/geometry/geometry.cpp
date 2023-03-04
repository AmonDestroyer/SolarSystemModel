#include "geometry/geometry.hpp"
#include <iostream>

// Constructor
Geometry::Geometry() {
}

// Private Methods
vector<Triangle> Geometry::SplitTriangle(vector<Triangle> &list) {
    vector<Triangle> output(4*list.size());
    output.resize(4*list.size());
    for (unsigned int i = 0 ; i < list.size() ; i++)
    {
        Triangle t = list[i];
        vec3 vmid1, vmid2, vmid3;
        vmid1 = (t.v0 + t.v1) / 2.0f;
        vmid2 = (t.v1 + t.v2) / 2.0f;
        vmid3 = (t.v0 + t.v2) / 2.0f;
        output[4*i+0].v0 = t.v0;
        output[4*i+0].v1 = vmid1;
        output[4*i+0].v2 = vmid3;
        output[4*i+1].v0 = t.v1;
        output[4*i+1].v1 = vmid2;
        output[4*i+1].v2 = vmid1;
        output[4*i+2].v0 = t.v2;
        output[4*i+2].v1 = vmid3;
        output[4*i+2].v2 = vmid2;
        output[4*i+3].v0 = vmid1;
        output[4*i+3].v1 = vmid2;
        output[4*i+3].v2 = vmid3;
    }
    return output;
}

void Geometry::PushVertex(vector<float>& coords, const vec3& v) {
    coords.push_back(v.x);
    coords.push_back(v.y);
    coords.push_back(v.z);
}

// Public Methods
void Geometry::GetCylinderData(vector<float>& coords, vector<float>& normals) {
    int nfacets = 30;
    for (int i = 0 ; i < nfacets ; i++) {
        double angle = 3.14159*2.0*i/nfacets;
        double nextAngle = (i == nfacets-1 ? 0 : 3.14159*2.0*(i+1)/nfacets);
        vec3 fnormal(0.0f, 0.0f, 1.0f); 
        vec3 bnormal(0.0f, 0.0f, -1.0f);
        vec3 fv0(0.0f, 0.0f, 1.0f);
        vec3 fv1(cos(angle), sin(angle), 1);
        vec3 fv2(cos(nextAngle), sin(nextAngle), 1);
        vec3 bv0(0.0f, 0.0f, 0.0f);
        vec3 bv1(cos(angle), sin(angle), 0);
        vec3 bv2(cos(nextAngle), sin(nextAngle), 0);
        // top and bottom circle vertices
        PushVertex(coords, fv0);
        PushVertex(normals, fnormal);
        PushVertex(coords, fv1);
        PushVertex(normals, fnormal);
        PushVertex(coords, fv2);
        PushVertex(normals, fnormal);
        PushVertex(coords, bv0);
        PushVertex(normals, bnormal);
        PushVertex(coords, bv1);
        PushVertex(normals, bnormal);
        PushVertex(coords, bv2);
        PushVertex(normals, bnormal);
        // curves surface vertices
        vec3 v1normal(cos(angle), sin(angle), 0);
        vec3 v2normal(cos(nextAngle), sin(nextAngle), 0);
        //fv1 fv2 bv1
        PushVertex(coords, fv1);
        PushVertex(normals, v1normal);
        PushVertex(coords, fv2);
        PushVertex(normals, v2normal);
        PushVertex(coords, bv1);
        PushVertex(normals, v1normal);
        //fv2 bv1 bv2
        PushVertex(coords, fv2);
        PushVertex(normals, v2normal);
        PushVertex(coords, bv1);
        PushVertex(normals, v1normal);
        PushVertex(coords, bv2);
        PushVertex(normals, v2normal);
    }
}


void Geometry::GetSphereData(vector<float>& coords, vector<float>& normals) {
    int recursionLevel = 3;
    std::vector<Triangle> list;
    {
        Triangle t;
        t.v0 = glm::vec3(1.0f,0.0f,0.0f);
        t.v1 = glm::vec3(0.0f,1.0f,0.0f);
        t.v2 = glm::vec3(0.0f,0.0f,1.0f);
        list.push_back(t);
    }
    for (int r = 0 ; r < recursionLevel ; r++)
    {
        list = SplitTriangle(list);
    }

    for (int octant = 0 ; octant < 8 ; octant++)
    {
        glm::mat4 view(1.0f);
        float angle = 90.0f*(octant%4);
        if(angle != 0.0f)
        view = glm::rotate(view, glm::radians(angle), glm::vec3(1, 0, 0));
        if (octant >= 4)
        view = glm::rotate(view, glm::radians(180.0f), glm::vec3(0, 0, 1));
        for(int i = 0; i < list.size(); i++)
        {
        Triangle t = list[i];
        float mag_reci;
        glm::vec3 v0 = view*glm::vec4(t.v0, 1.0f);
        glm::vec3 v1 = view*glm::vec4(t.v1, 1.0f);
        glm::vec3 v2 = view*glm::vec4(t.v2, 1.0f);
        mag_reci = 1.0f / glm::length(v0);
        v0 = glm::vec3(v0.x * mag_reci, v0.y * mag_reci, v0.z * mag_reci);
        mag_reci = 1.0f / glm::length(v1);
        v1 = glm::vec3(v1.x * mag_reci, v1.y * mag_reci, v1.z * mag_reci);
        mag_reci = 1.0f / glm::length(v2);
        v2 = glm::vec3(v2.x * mag_reci, v2.y * mag_reci, v2.z * mag_reci);
        PushVertex(coords, v0);
        PushVertex(coords, v1);
        PushVertex(coords, v2);
        PushVertex(normals, v0);
        PushVertex(normals, v1);
        PushVertex(normals, v2);
        }
    }
}