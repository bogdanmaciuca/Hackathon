#pragma once

#define DIRECTION_LEFT  0
#define DIRECTION_RIGHT 1

class Player {
private:
	float m_x = 0, m_y = 0;
	Graphics* gfx;
	char* mode;
	char m_anim_frame_num_arr[6] = { 8, 12, 1, 8, 12, 1 };
	Graphics::ComplexAnimation m_animation;
	float m_velo_x = 0, m_velo_y = 0;
	float m_timejump = 21, grav = 0.25f, jumpforce = -7.0f, adjustjump = 35.0f;
	float m_max_horiz_vel = 5.0f;
	float accel = 0.08f;
	float decel = 0.1f;
	bool direction = DIRECTION_RIGHT;
	bool ground = 0;
	bool aux_jump_cmd = 0;
public:
	float GetX() { return m_x; }
	float GetY() { return m_y; }
	void Reset() {
		m_x = 0, m_y = -m_animation.sprite.height * 2;
		m_velo_x = 0, m_velo_y = 0;
	}
	void SetNormalStats() {
		m_timejump = 21, grav = 0.25f, jumpforce = -7.0f, adjustjump = 35.0f;
		m_max_horiz_vel = 5.0f;
		accel = 0.08f;
		decel = 0.1f;
	}
	void SetSlipperyStats() {
		accel = 0.01f;
		decel = 0.005f;
	}
	void SetRandomJumpStats() {
		SetNormalStats();
		jumpforce = -10.0f;
	}
	Player(Graphics* graphics, char* _mode) {
		gfx = graphics;
		mode = _mode;
		gfx->LoadComplexAnim(L"res/player_anim.png", &m_animation, 100.0f, 6, m_anim_frame_num_arr);
		//gfx->SetComplexAnimState(&m_animation, 0);
		m_animation.sprite.width = PLAYER_WIDTH;
		m_animation.sprite.height = PLAYER_HEIGHT;
		Reset();
	}
	void Jump() { // "You gotta' jump!"
		aux_jump_cmd = 1;
	}
	void UpdateInput(Input* input) {
		if (input->GetKeyDown('A')) m_velo_x -= accel;
		else if (input->GetKeyDown('D')) m_velo_x += accel;
		if (!input->GetKeyDown('A') && !input->GetKeyDown('D'))
		{
			if (m_velo_x > 0)
				m_velo_x -= decel;
			else
				m_velo_x += decel;
		}
		if (m_velo_x <= -m_max_horiz_vel)
			m_velo_x = -m_max_horiz_vel;
		if (m_velo_x >= m_max_horiz_vel)
			m_velo_x = m_max_horiz_vel;
	}
	void UpdateGravity(const std::vector<Platform>& platforms, Input* input, float deltaTime) {
		bool colision = 0;
		ground = 0;
		int i;
		for (i = 0; i < 10 && !colision; i++) {
			if (m_x < platforms[i].x + platforms[i].length * PLAYER_WIDTH &&
				m_x + PLAYER_WIDTH > platforms[i].x &&
				m_y < platforms[i].y + PLAYER_WIDTH &&
				(PLAYER_WIDTH + m_y > platforms[i].y || PLAYER_WIDTH + m_y + m_velo_y > platforms[i].y))
			{
				colision = 1;
			}
		}
		i--;

		if (colision && (PLAYER_WIDTH + m_y > platforms[i].y + 2.0f)) ground = 0;
		else if (colision) ground = 1;
		
		if (ground) {
			m_velo_y = 0;
			if (aux_jump_cmd || (*mode != MODE_RANDOM_JUMP && input->GetKeyPressed(VK_SPACE))) {
				m_velo_y = jumpforce;
				m_timejump = 0;
				ground = false;
				aux_jump_cmd = 0;
			}
		}
		else if (!(input->GetKeyDown(VK_SPACE) && m_timejump < 3))
			m_velo_y += grav;
		else
			m_timejump += deltaTime / adjustjump;
		
		m_x += m_velo_x * deltaTime / 7.5;
		m_y += m_velo_y * deltaTime / 7.5;
	}
	void UpdateAnimationState() {
		if (!ground) { // Jumping
			if (m_velo_x < 0) {
				gfx->SetComplexAnimState(&m_animation, 5);
				direction = DIRECTION_LEFT;
			}
			else {
				gfx->SetComplexAnimState(&m_animation, 2);
				direction = DIRECTION_RIGHT;
			}
		}
		else {
			if (abs(m_velo_x) < m_max_horiz_vel / 10.0f) { // Idle
				if (direction == DIRECTION_LEFT)
					gfx->SetComplexAnimState(&m_animation, 4);
				else
					gfx->SetComplexAnimState(&m_animation, 1);
			}
			else { // Runnning
				if (m_velo_x < 0) {
					gfx->SetComplexAnimState(&m_animation, 3);
					direction = DIRECTION_LEFT;
				}
				else {
					gfx->SetComplexAnimState(&m_animation, 0);
					direction = DIRECTION_RIGHT;
				}
			}
		}
	}
	void Draw(const float& delta_time) {
		gfx->DrawComplexAnimFrame(&m_animation, delta_time, m_x, m_y);
	}
};