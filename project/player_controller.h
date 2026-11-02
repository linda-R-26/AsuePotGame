#pragma once
struct PlayerController { enum State { Idle,Attack,Hit,Fail,Win };State state=Idle;void update(GameManager&g){state=g.state==GameManager::FAIL?Fail:g.state==GameManager::WIN?Win:g.anim?(g.impacted?Hit:Attack):Idle;} };
