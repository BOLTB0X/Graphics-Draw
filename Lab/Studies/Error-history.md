# Error 및 궁금증들 기록장

에러는 `E숫자` , 궁금해서 찾아본건 `Q숫자`로 구분

주로 gemini, Gpt 등 ~~(무료버전)~~ 과 구글링으로 나름대로 해결을 기록 

## Error

### E1. 삼각형이 사방팔방으로 튀기다 꺼지는 문제

> **화면이 깨지는 현상** 은 GPU가 버텍스 데이터를 읽을 때,<br/> **좌표값을 읽어야 할 자리에서 텍스처 좌표나 노멀 값을 읽어버려서 삼각형이 사방팔방으로 튀기 때문에 발생**

GPU에게 이 구조체의 생김새를 알려주는(Input Layout) 과정 사이의 미세한 불일치 때문

-> 나의 해결법: 기존 셰이더 관련 구조체나 바인딩 부분을 셰이더에 맞게 했는지 확인 및 버퍼 설정등을 확인해봐야함

ex. *삼각형 리스트(Triangle List)로 그리기로 변경하면서 한 사각형 당 정점이 6개가 되었으나, `InitBuffers` 내부 계산식에는 예전 방식인 8개(Line List용)가 그대로 남아 있었음.*

---

### E2. 이중 해제(Double Free)

프로그램 종료 시 `ucrtbased.dll`에서 액세스 위반이 발생했던 문제

- **원인**

    `Texture::Init` 함수 끝에서 `stbi_image_free(imageData)`를 호출해 메모리를 해제했지만,
    
    포인터 변수(`imageData`)를 `nullptr`로 초기화하지 않음

    프로그램 종료 시 `Texture::Shutdown` 에서 `if (imageData)` 조건문을 통과하게 되고
    
    이미 해제된 주소를 다시 `stbi_image_free`하려 시도하면서 런타임 에러 발생

- **해결**

`stbi_image_free(imageData)` 호출 직후 반드시 `imageData = nullptr;` 를 대입

이렇게 하면 `Shutdown` 시점에 조건문(`if (imageData)`)을 통과하지 못하게 되어 안전하게 중복 해제를 방지함

~~현재는 그냥 스마트포인터 사용~~

---

## 궁금했던 것들

### Q1. 그래픽스 분야에선 상속/추상화보다 의존성(Composition) 관리를 선호하나?

> 그래픽스 및 게임 엔진 개발에서는 "Composition over Inheritance(상속보다는 합성)" 원칙을 훨씬 강력하게 적용함

- **상속의 위험성**

    ```
    WorldObject → PhysicalObject → Terrain
    ```

    위처럼 상속 계층이 깊어지면, 나중에 "물리는 없는데 렌더링만 되는 객체"
    
    혹은 "렌더링은 없는데 소리만 나는 객체"를 만들 때 구조가 꼬여버림
    
    (이를 다이아몬드 상속 문제라 함)

    ---

- **컴포넌트 방식**

    현대 엔진(Unity, Unreal)은 객체를 상속하지 않고,
    
    `GameObject` 라는 빈 껍데기에 `MeshComponent`, `PhysicsComponent`, `LightComponent`를 **포함(Include/Member)**시켜서 기능을 조립

    ---

- **성능(캐시 효율성)**

    너무 잦은 상속을 통한 `virtual` 호출은 **V-Table 오버헤드** 를 발생시킬수 있음
    
    수천 개의 객체를 렌더링해야 하는 그래픽스 분야에서는 멤버 변수로 클래스를 가지고 있거나,
    
    아예 독립적인 시스템이 데이터를 순차적으로 처리하는 방식을 선호

    ---

- 그럼 상속이나 맴버로 언제 쓰나

    - **상속을 써도 되는 경우 (아주 제한적)**
    
        “이것은 이것이다 (is-a)” 가 절대적으로 성립할 때

    - **멤버로 둬야 하는 경우**
    
        “이것은 이것을 사용한다 (has-a)”

---

### Q2. Shader는 컴포넌트보다는 '참조'가 적합한가?

Shader는 *"행동이나 상태를 가진 존재”* 가 아니라

*“데이터를 해석하는 규칙”* 이기 때문에 컴포넌트가 아니라 참조가 맞다고 함

- 컴포넌트의 공통 속성

    - Actor에 속함 (`Transform / SceneComponent`)

    - Actor의 생명주기를 따름 (`RigidBody / Collider`)

    - Actor마다 개별 인스턴스를 가질 수 있음(`CameraComponent`)

    - 보통 월드 상태를 직접 바꾸기 가능(`Script` / `Behaviour`)

    ---

Shader는 Actor의 상태를 바꾸지 않고 단지 GPU가 데이터를 해석하는 방법만 제공함

```
//Shader 구조
Material Function / Render Rule / Interpretation Logic
```

-> **"이 데이터를 어떻게 화면에 그릴 것인가"** 가 관건

---

### Q3. 리소스와 생명주기

Q. *컴포넌트의 공통 특징 상 마야모델, 텍스처, 셰이더를 `Application`에서 로드 하고 포인터로 `World`로 보내어 `WorldObject`에 전달해 주면 `Application` 상의 내용들과 생명주기가 달라서 문제가 되질않나?*

#### 1 "생명주기를 따른다"는 규칙은 `Position` 같은 순수 컴포넌트에 해당

-> `Model`은 컴포넌트가 아니라 액터가 사용하는 외형 리소스

- **순수 컴포넌트** (ex. `Position`)    

    - `ActorObject`가 `unique_ptr`로 소유

    - 액터가 죽으면 같이 죽음 (생명주기 일치)

- **리소스 참조 (Model)**

    - 액터는 주소값(Raw Pointer)만 들고 있는 것
    
    - 액터가 죽어도 원본 데이터는 ModelManager 안에 살아있음(생명주기 독립)

    ---

#### 2. 그럼 왜 이렇게 해야 하나?

-> 내가 생각했던 것처럼 데이터를 캐시(Cache) 처리해 두었기 때문에 괜찮은 것

- **재사용성**

    만약 액터가 죽을 때 모델 데이터까지 날려버리면,
    
    잠시 후 똑같은 `Stone`을 소환할 때 Assimp로 다시 파일을 읽고 GPU 메모리에 올리는 비싼 비용이 발생

    ---

- **안전성**

    `ModelManager`가 `unique_ptr 맵`으로 모델을 꽉 잡고 있기 때문에, `WorldObject`가 수만 번 생성되고 해제되어도 실제 메모리에 로드된 정점 데이터는 하나뿐이며 절대 유실되지 않음

#### 3. "Render 취소"에 대한 걱정

*"Actor가 소멸될 때 m_model.render를 취소해야 하나?"*

렌더링은 매 프레임 살아있는 객체들만 모아서 수행되기 때문에 그럴 필요가 X

`World`가 소유한 `m_actors` 리스트에서 특정 액터가 제거(Delete)됨

다음 프레임의 `World::Render` 루프에서는 그 액터가 리스트에 없으므로, 자연스럽게 해당 모델의 `Render()` 함수 자체가 호출되지 않음

데이터는 메모리에 남아있지만(캐시), GPU는 명령을 받지 않으므로 그리기를 멈춤

#### 4. Smart Pointer 활용

- `std::shared_ptr<Model>`: 매니저와 액터가 소유권을 공유

- `std::weak_ptr<Model>`: 매니저는 가지고만 있고, 액터는 필요할 때만 체크해서 씀

이런 방식을 잘 조합해서 사용해야함

---
