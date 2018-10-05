ptask
====
Priority based scheduler for game engine.<Br>
<sup>friend of the lemon</sup>

Goal
----
* priority based `update`
* __[LOD (Animation)](https://youtu.be/X3htj7jUBeg?t=3m16s)__

Example
----
__the `slowcow` object__
```cpp
class slow_cow : public cocos2d::Sprite {
public:
    /* ... */
	virtual void update(float dt) {
		elapsed += dt;
        
                // animation things
		setPositionX(sin(elapsed) * 500 + offsetX);

                // This simulates some heavy task....
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
 Just update in every frames. (defaults for cocos2d-x)<br>
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
__Low Priority__ : Objects with priority are scheduled in a range that does not unconditionally drop the frame.<br>
![p](img/ptask1.gif)<br>
<br><br>
__Normal Priority__ : Prefers the framerate, but if they are stuck for too long, the priority is increased. (Possible to reduce framerates.)<br>
![p](img/ptask2.gif)<br>
<br>
* The big ball is scheduled by cocos2d-x, not a ptask. You can estimate the framerate with it.

Things to do
----
아직 우선순위 시스템 덜만듬<br>
[레몬에서의 시분할 디자인](https://github.com/pjc02478/lemon/blob/master/example/physics.cpp)

ptask::_for
----
scheduled __for__, low stress instantiation<br>
This prevents frame-drop caused by instantiating tons of objects at once.
```cpp
ptask::_for(
    1, 100,
    [](int i) {
        // SOME HEAVY TASKS
    });
```

Sample project
----
![ga](img/galaxy.gif)
