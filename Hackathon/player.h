#pragma once

#define DIRECTION_LEFT 0
#define DIRECTION_RIGHT 1

class Player {
private:
	float m_x = 0, m_y = 0;
	Graphics* gfx;
	char* mode;
	char m_anim_frame_num_arr[6] = { 8, 12, 1, 8, 12, 1 };
	Graphics::ComplexAnimation m_animation;
	float m_velocity_x = 0, m_velocity_y = 0;
	float m_jump_velocity = -1.0f;
	bool m_holding_jump = 0;
	float m_hold_jump_timer = 0.0f;
	float m_max_hold_jump_time = 300.0f;
	float m_gravity = 0.01f;
	bool m_ground = 0;
	bool m_aux_jump_cmd = 0;

	float m_max_horiz_vel = 5.0f;
	float m_accel = 0.08f;
	float m_decel = 0.1f;
	bool m_direction = DIRECTION_RIGHT;
public:
	float GetX() { return m_x; }
	float GetY() { return m_y; }
	void Reset() {
		m_x = 0, m_y = -m_animation.sprite.height * 2;
		m_velocity_x = 0, m_velocity_y = 0;
	}
	void SetNormalStats() {
		m_jump_velocity = -1.0f;
		m_max_horiz_vel = 5.0f;
		m_max_hold_jump_time = 300.0f;
		m_accel = 0.08f;
		m_decel = 0.1f;
	}
	void SetSlipperyStats() {
		SetNormalStats();
		m_accel = 0.01f;
		m_decel = 0.005f;
	}
	void SetRandomJumpStats() {
		SetNormalStats();
		m_jump_velocity = -1.5f;
	}
	void SetBunnyhopStats() {
		SetNormalStats();
		m_jump_velocity= -1.5f;
		m_accel = 0.16f;
		m_decel = 0.15f;
	}
	void SetNoLeftStats() {
		SetNormalStats();
		m_jump_velocity = -1.5f;
		m_decel = 0.05f;
	}
	void SetBigJumpsStats() {
		SetNormalStats();
		m_gravity = 0.005f;
		m_jump_velocity = -1.5f;
	}
	void SetGlidingStats() {
		SetNormalStats();
	}
	void SetSpeedStats() {
		SetNormalStats();
		m_max_horiz_vel = 10.0f;
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
		m_aux_jump_cmd = 1;
	}
	void UpdateInput(Input* input, bool no_left = 0) {
		if (!no_left && input->GetKeyDown('A')) m_velocity_x -= m_accel;
		else if (input->GetKeyDown('D')) m_velocity_x += m_accel;
		if (!(!no_left && input->GetKeyDown('A')) && !input->GetKeyDown('D')) {
			if (m_velocity_x > 0)
				m_velocity_x -= m_decel;
			else
				m_velocity_x += m_decel;
			if (abs(m_velocity_x) <= 2 * m_decel)
				m_velocity_x = 0;
		}
		if (m_velocity_x <= -m_max_horiz_vel)
			m_velocity_x = -m_max_horiz_vel;
		if (m_velocity_x >= m_max_horiz_vel)
			m_velocity_x = m_max_horiz_vel;
	}
	void UpdateGravity(const std::vector<Platform>& platforms, Input* input, float delta_time, bool random_jump) {
		bool colision = 0;
		
		float ground_height = 2000.0f;
		int i;
		for (i = 0; i < PLATFORM_NUM_ON_SCR && !colision; i++) {
			if (m_x < platforms[i].x + platforms[i].length * PLAYER_WIDTH &&
				m_x + PLAYER_WIDTH > platforms[i].x)
			{
				colision = 1;
				ground_height = platforms[i].y;
			}
		}
		if (ground_height == 2000.0f) m_ground = 0;
		i--;

		if (m_ground) {
			if (m_aux_jump_cmd || (!random_jump && input->GetKeyPressed(VK_SPACE))) {
				m_ground = 0;
				m_velocity_y = m_jump_velocity;
				m_holding_jump = 1;
				m_hold_jump_timer = 0.0f;
				m_aux_jump_cmd = 0;
			}
		}
		if (input->GetKeyReleased(VK_SPACE)) {
			m_holding_jump = 0;
		}

		if (!m_ground) {
			if (m_holding_jump) {
				m_hold_jump_timer += delta_time;
				if (m_hold_jump_timer >= m_max_hold_jump_time) {
					m_holding_jump = 0;
				}
			}
			m_y += m_velocity_y * delta_time;
			if (!m_holding_jump) {
				m_velocity_y += m_gravity * delta_time;
			}
			if (m_y >= ground_height - PLAYER_HEIGHT && m_y <= ground_height + PLAYER_HEIGHT / 2) {
				m_y = ground_height - PLAYER_HEIGHT;
				m_ground = 1;
			}
		}

		m_x += m_velocity_x * delta_time / 7.5;
	}
	void UpdateAnimationState() {
		if (!m_ground) { // Jumping
			if (m_velocity_x < 0) {
				gfx->SetComplexAnimState(&m_animation, 5);
				m_direction = DIRECTION_RIGHT;
			}
			else {
				gfx->SetComplexAnimState(&m_animation, 2);
				m_direction = DIRECTION_RIGHT;
			}
		}
		else {
			if (abs(m_velocity_x) < m_max_horiz_vel / 10.0f) { // Idle
				if (m_direction == DIRECTION_LEFT)
					gfx->SetComplexAnimState(&m_animation, 4);
				else
					gfx->SetComplexAnimState(&m_animation, 1);
			}
			else { // Runnning
				if (m_velocity_x < 0) {
					gfx->SetComplexAnimState(&m_animation, 3);
					m_direction = DIRECTION_LEFT;
				}
				else {
					gfx->SetComplexAnimState(&m_animation, 0);
					m_direction = DIRECTION_RIGHT;
				}
			}
		}
	}
	void Draw(const float& delta_time) {
		gfx->DrawComplexAnimFrame(&m_animation, delta_time, m_x, m_y);
	}
};