// src/World/Actors/Stone/MyStone.h
#pragma once
#include "Framework/ActorObject.h"

class MyStone : public ActorObject {
private:
    float m_rollSpeed; // 돌이 구르는 속도
    float m_rotationAmount; // 현재 회전 누적값

public:
    MyStone();
    virtual ~MyStone() override;

    // 초기화: 부모의 모델을 세팅하고 돌 특유의 데이터 설정
    virtual bool Init(std::shared_ptr<MyModel> model) override;
    // 업데이트: 여기서 돌이 굴러가는 수학적 로직 처리
    virtual void Update(float dt) override;

    // 렌더링: 모델을 그리기 위한 준비 및 드로우 콜
    virtual void Render(ID3D11DeviceContext* context) override;
};