#include "MyStone.h"

MyStone::MyStone() : m_rollSpeed(50.0f), m_rotationAmount(0.0f) {}

bool MyStone::Init(std::shared_ptr<MyModel> model) {
    if (!model) return false;
    m_model = model;

    // 초기 위치나 스케일 설정
    m_position.SetPosition(0.0f, 1.0f, 0.0f); // 지면보다 약간 위
    m_position.SetScale(1.0f, 1.0f, 1.0f);

    return true;
}

void MyStone::Update(float dt) {
    // 1. 시간이 지남에 따라 회전값 누적
    m_rotationAmount += m_rollSpeed * dt;
    if (m_rotationAmount > 360.0f) m_rotationAmount -= 360.0f;

    // 2. Position 객체에 회전 적용 (X축으로 데굴데굴)
    // m_position 내부의 m_isDirty가 true가 되어 나중에 행렬을 새로 계산함
    m_position.SetRotation(m_rotationAmount, 0.0f, 0.0f);

    // 3. 앞으로 이동하는 로직이 있다면 여기에 추가
    // float currentZ = m_position.GetPosition().z;
    // m_position.SetPosition(0, 1, currentZ + (10.0f * dt));
}

void MyStone::Render(ID3D11DeviceContext* context) {
    if (!m_model) return;

    // 1. 셰이더에게 넘겨줄 월드 행렬 준비 (Position이 관리)
    // DirectX::XMMATRIX world = m_position.GetWorldMatrix();

    // 2. 모델 버퍼 바인딩
    m_model->Render(context);

    // 3. 실제 그리기 (DrawIndexed)는 Shader 클래스에서 하거나 여기서 직접 수행
    // context->DrawIndexed(m_model->GetIndexCount(), 0, 0);

}