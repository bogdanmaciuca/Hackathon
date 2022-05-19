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
	float m_jump_velocity = -1.25f;
	bool m_holding_jump = 0;
	float m_hold_jump_timer = 0.0f;
	float m_max_hold_jump_time = 175.0f;
	float m_gravity = 0.008f;
	bool m_ground = 0;
	bool m_aux_jump_cmd = 0;

	float m_max_horiz_vel = 7.0f;
	float m_accel = 0.08f;
	float m_decel = 0.1f;
	float m_aux_horiz_accel = 0.0f;
	bool m_direction = DIRECTION_RIGHT;

	char *controls;
public:
	float GetX() { return m_x; }
	float GetY() { return m_y; }
	void Reset(bool anim = 0) {
		m_x = 0, m_y = -m_animation.sprite.height * 2;
		m_velocity_x = 0, m_velocity_y = 0;

		float black[] = { 0,0,0,0 }, tr = 0;
		Graphics::Sprite background;
		gfx->LoadSprite(L"res/background.png", &background);
		background.width = gfx->GetWindowWidth();
		background.height = gfx->GetWindowHeight();
		gfx->SetCamera(gfx->GetWindowWidth() / 2, gfx->GetWindowHeight() / 2);
		bool f_in = 1;
		float tim = 0.02f;
		while (black[3] >= 0 && anim) {
			gfx->HandleMessages();
			gfx->BeginFrame();
			gfx->DrawSprite(background, 0, 0);
			gfx->DrawRect(0, 0, 800, 600, black, 0, true);
			if (f_in) {
				if (black[3] + tim <= 1) black[3] += tim;
				else f_in = 0;
			}
			else black[3] -= tim;
			gfx->EndFrame();
		}
	}
	void SetNormalStats() {
		m_jump_velocity = -1.0f;
		m_max_horiz_vel = 7.0f;
		m_max_hold_jump_time = 300.0f;
		m_accel = 0.08f;
		m_decel = 0.1f;
		m_aux_horiz_accel = 0.0f;
		m_gravity = 0.008f;
		m_animation.time_per_frame = 50.0f;
	}
	void SetSlipperyStats() {
		SetNormalStats();
		m_accel = 0.04f;
		m_decel = 0.01f;
	}
	void SetRandomJumpStats() {
		SetNormalStats();
		m_jump_velocity = -1.1f;
		m_accel = 0.18f;
	}
	void SetBunnyhopStats() {
		SetNormalStats();
		m_max_horiz_vel = 7.5f;
		m_jump_velocity= -0.9f;
		m_accel = 0.18f;
		m_decel = 0.15f;
	}
	void SetNoLeftStats() {
		SetNormalStats();
		m_jump_velocity = -1.1f;
		m_max_horiz_vel = 7.0f;
		m_decel = 0.05f;
	}
	void SetBigJumpsStats() {
		SetNormalStats();
		m_gravity = 0.009f;
		m_jump_velocity = -2.0f;
	}
	void SetGlidingStats() {
		SetNormalStats();
	}
	void SetSpeedStats() {
		SetNormalStats();
		m_max_horiz_vel = 8.0f;
		m_accel = 0.05f;
		m_decel = 0.15f;
		m_animation.time_per_frame = 40.0f;
	}
	Player(Graphics* graphics, char* _mode, char* _controls) {
		gfx = graphics;
		mode = _mode;
		controls = _controls;
		gfx->LoadComplexAnim(L"res/player_anim.png", &m_animation, 50.0f, 6, m_anim_frame_num_arr);
		//gfx->SetComplexAnimState(&m_animation, 0);
		m_animation.sprite.width = PLAYER_WIDTH;
		m_animation.sprite.height = PLAYER_HEIGHT;
		Reset();
	}
	void Jump() { // "You gotta' jump!"
		m_aux_jump_cmd = 1;
	}
	void SetLowerGravity() {
		m_gravity = 0.0025f;
	}
	void SetNormalGravity() {
		m_gravity = 0.008f;
	}
	void SetWindStats() {
		SetNormalStats();
		m_aux_horiz_accel = m_accel * 0.75f;
		m_velocity_x = 8.0f;
	}
	void UpdateInput(Input* input, bool no_left = 0) {
		if (!no_left && input->GetKeyDown(controls[0])) m_velocity_x -= m_accel;
		else if (input->GetKeyDown(controls[1])) m_velocity_x += m_accel;
		if (!(!no_left && input->GetKeyDown(controls[0])) && !input->GetKeyDown(controls[1])) {
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
		m_velocity_x += m_aux_horiz_accel;
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
			if (m_aux_jump_cmd || (!random_jump && input->GetKeyPressed(controls[2]))) {
				m_ground = 0;
				m_velocity_y = m_jump_velocity;
				m_holding_jump = 1;
				m_hold_jump_timer = 0.0f;
				m_aux_jump_cmd = 0;
			}
		}
		if (input->GetKeyReleased(controls[2])) {
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
			if (m_y >= ground_height - PLAYER_HEIGHT && m_y <= ground_height + PLAYER_HEIGHT / 4) {
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