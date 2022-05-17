
class Player {
private:
	float m_x = 0, m_y = 0.f;
	Graphics* gfx;
	Graphics::Animation m_animation;
	const int contr = 69;
	float m_velo_x = 0, m_velo_y= 0 ;
	float m_timejump = 21, grav = 0.1f, jumpforce = -4.0f, adjustjump = 75.0f;
public:
	float GetX() { return m_x; }
	float GetY() { return m_y; }
	Player(Graphics* graphics) {
		gfx = graphics;
		gfx->LoadAnimation(L"res/player_anim.png", &m_animation, 100.0f, 8);
		m_animation.sprite.width = 69;
		m_animation.sprite.height = 69;
		m_y = -m_animation.sprite.height * 2;
	}
	void UpdateInput(Input* input) {
		if (input->GetKeyDown('A'))
			m_velo_x -= 0.03f;
		if (input->GetKeyDown('D')) m_velo_x += 0.03f;
		if (!input->GetKeyDown('A') && !input->GetKeyDown('D'))
		{
			if(m_velo_x>0)
				m_velo_x -= 0.03f;
			else
				m_velo_x += 0.03f;
		}
		if (m_velo_x <= -2)
			m_velo_x = -2;
		if (m_velo_x >= 2)
			m_velo_x = 2;
		
	}
	void UpdateGravity(const std::vector<Platform>& platforms, Input* input, float deltaTime) {
		/// !!!! Delta time
		bool colision = 0, ground = 0;
		int i;
		for (i = 0; i < 1024 && !colision; i++)
		{
			if (m_x < platforms[i].x + platforms[i].length * contr &&
				m_x + contr > platforms[i].x &&
				m_y < platforms[i].y + contr &&
				(contr + m_y > platforms[i].y || contr + m_y + m_velo_y > platforms[i].y))
				colision = 1;
		}
		i--;
			if (colision && (contr + m_y > platforms[i].y+ 2.0f))
				 ground = 0;
			else if(colision)
				ground = 1;	
					
			if (ground)
			{
				m_velo_y = 0;
				if (input->GetKeyPressed(VK_SPACE))
				{
					m_velo_y = jumpforce;
					m_timejump = 0;
					ground = false;
				}
			}
			else if (!(input->GetKeyDown(VK_SPACE) && m_timejump < 3))		
				m_velo_y += grav;			
			else
				m_timejump += deltaTime/adjustjump;
		m_x += m_velo_x*deltaTime / 7.5;
		m_y += m_velo_y * deltaTime / 7.5;
	}
	void Draw(const float& delta_time) {
		gfx->DrawAnimationFrame(&m_animation, delta_time, m_x, m_y);
	}
};
