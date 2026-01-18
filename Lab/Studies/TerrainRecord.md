# 지형 구현 방식 변경 과정

## 1. 배경 (초기 계획)

```cpp
class MeshModel {
public:
    MeshModel();
	MeshModel(const MeshModel&) = delete;
	~MeshModel();

    void Render(ID3D11DeviceContext* context, Shader* shader, Frustum* frustum, DirectX::XMMATRIX worldMatrix);

public:
    void AddMesh(std::unique_ptr<Mesh>);
    void AddMaterial(const Material&);

    bool InitConstantBuffer(ID3D11Device*);
    void GetPhysicsData(std::vector<ModelVertex>& outVertices, std::vector<unsigned int>& outIndices) const;
    float GetBottomOffset();
    float GetMinWidth() const;
    float GetMaxWidth() const;
    float GetMinHeight() const;
    float GetMaxHeight() const;
    float GetMinDepth() const;
    float GetMaxDepth() const;

private:
    void GetBoundingBox(DirectX::XMFLOAT3&, DirectX::XMFLOAT3&) const;

public:
    std::vector<MeshData> GetFullMeshData() const;

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<Material> m_materials;
    std::unique_ptr<ConstantBuffer<MaterialBuffer>> m_materialBuffer;
}; // MeshModel
```

처음에는 `MeshModel` (정적 FBX/OBJ)을 불러와서 지형으로 사용하려 함

cf. 보통 일반적인 게임의 딱딱한 땅(고정된 지형)을 만들 때 사용하는 표준 방식

---

## 2. `MeshModel` 방식의 한계점

### 1) 정적인 형태 (Static)

`MeshModel` 은 한 번 로드되면`Vertex` 의 위치를 매 프레임 CPU에서 수정하기가 매우 무거움

### 2) 메모리 및 성능

1024x1024 같은 넓은 범위를 세밀하게 표현하려면 수만 개의 폴리곤이 필요함 근데 이를 `MeshModel` 로 관리하면 성능 저하가 심
각함


## 3. 전환: 함수 기반(FBM) 셰이더 방식 채택

### 수학적 정의 (Deterministic)

`fbm(x, z, time)` 이라는 함수 하나로 GPU(셰이더)와 CPU(물리)가 동일한 높이값을 계산할 수 있게 되었음

GPU에서 Vertex 을 직접 들어 올리므로 CPU에 부담을 주지 않음

### 무한한 확장성

텍스처 데이터 없이 수식만으로 지형을 생성하므로,

메모리 사용량이 극히 적고 노이즈 수치만 조절하면 언제든 지형의 느낌(둥글게 혹은 뾰족하게)을 바꿀 수 있음

---