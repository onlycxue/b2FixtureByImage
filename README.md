b2FixtureByImage
================

1.此工程是一个cocos2dx-3.1.1的工程。
2.实现功能通过png图片自动创建搬b2Fixture.

处理思路
1.通过 marching squares algorithm 算法得出png图片的轮廓。

2.然后通过 Ramer-Douglas-peucker algorithm 裁剪点的数量。

3.通过 b2Separator 给b2Body 创建多个Fixture

参考文档：


http://www.emanueleferonato.com/2013/03/13/from-png-to-box2d-first-attempt/

bug：
存在创建b2Fixture 失败的点。
