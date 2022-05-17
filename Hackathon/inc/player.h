
class Player {
private:
	float m_x = 0, m_y = 0;
	Graphics* gfx;
	Graphics::Animation m_animation;
public:
	float GetX() { return m_x; }
	float GetY() { return m_y; }
	Player(Graphics* graphics) {
		gfx = graphics;
		gfx->LoadAnimation(L"player_anim.png", &m_animation, 100.0f, 8);
		m_animation.sprite.width = 69;
		m_animation.sprite.height = 69;
		m_y = -m_animation.sprite.height * 2;
	}
	void UpdateInput(Input* input) {
		if (input->GetKeyDown('W')) m_y -= 1.5f;
		if (input->GetKeyDown('S')) m_y += 1.5f;
		if (input->GetKeyDown('A')) m_x -= 1.5f;
		if (input->GetKeyDown('D')) m_x += 1.5f;
	}
	void UpdateGravity() {

	}
	void Draw(const float& delta_time) {
		gfx->DrawAnimationFrame(&m_animation, delta_time, m_x, m_y);
	}
};
