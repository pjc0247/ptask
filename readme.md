ptask
====
시분할 방식의 작업 스케줄러<Br>
<sup>레몬친구</sup>

Goal
----
* priority based `update`
* __[LOD (Animation)](https://youtu.be/X3htj7jUBeg?t=3m16s)__

Sample
----
__the `slowcow` object__
```cpp
class slow_cow : public cocos2d::Sprite {
public:
    /* ... */
	virtual void update(float dt) {
		elapsed += dt;
        
        // 애니메이션
		setPositionX(sin(elapsed) * 500 + offsetX);

        // 무거운 작업
		for (int i = 0; i < 500000; i++);
	}
    /* ... */
};
```

__without PTask__<br>
 ```cpp
 for (int i = 0; i < 100; i++) {
	auto node = new slow_cow;
	node->setPosition(Vec2(0, 300 + i));
	node->init();
	node->autorelease();
	
	node->scheduleUpdate();
	addChild(node);
}
 ```
 그냥 매 프레임마다 업데이트한다. 코코스 기본<br>
 ![p](img/without_ptask.gif)<br>
 <br>
 __with PTask__<br>
 ```cpp
 for (int i = 0; i < 100; i++) {
	auto node = new slow_cow;
	node->setPosition(Vec2(0, 300 + i));
	node->init();
	node->autorelease();
	
	ptask::_schedule_update(node);
	addChild(node);
}
 ```
우선순위 최하위 : 이 레벨의 오브젝트는 프레임을 무조건 떨어트리지 않는 범위에서 스케쥴링된다.<br>
![p](img/ptask1.gif)<br>
<br><br>
우선순위 보통 : 평소엔 프레임이 우선시되지만, 너무 오래 정체되었을 경우 강제로 우선순위를 높인다. 주기적인 프레임 감소 가능성이 있음.<br>
![p](img/ptask2.gif)<br>
<br>
* 아래 큰공은 전체 프레임레이트를 보여주기 위한 오브젝트. Ptask 스케줄링 아님 (코코스 기본)

ㅇㅁㄴㅇ
----
아직 우선순위 시스템 덜만듬<br>
[레몬에서의 시분할 디자인](https://github.com/pjc02478/lemon/blob/master/example/physics.cpp)

ptask::_for
----
분할 for, 부담을 주지 않는 초기화<br>
한번에 오브젝트를 너무 많이 찍어낼 경우 순간적으로 프레임이 저하되는 현상을 방지
```cpp
ptask::_for(
    1, 100,
    [](int i) {
        // for의 본문
    });
```