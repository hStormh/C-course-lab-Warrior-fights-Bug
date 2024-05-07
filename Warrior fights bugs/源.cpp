// hover ��ͣ

/*
**ע�����

��Ŀ���ƣ�Warrior fights boars
��Ŀ���ڣ�5��
��Ŀ����ԭ��

������Ϸ����Ļ�����ܣ�
��ʼ������
��ѭ��{
��ȡ����������
�������ݣ�����
���ƻ��棨����
�����˼�룬ʹ��ȡ�ʹ������ݵ��߼��뻭����Ⱦ���롣
}
�ͷ���Դ������
*/

//����ԭ���ȴ��ܣ�����ϸ����ע�ʹ��档


#include <graphics.h>
#include <windows.h>
#include <string>
#include <vector> //��׼ģ����vector����
#include <easyx.h>
#include <tchar.h> //����_T��

#pragma comment(lib, "Winmm.LIB")
#pragma comment(lib, "MSIMG32.LIB")

//���û��Ҫ���ֻ�Ƿ�װ��һ������ɫ͸�����ֵļ���ͼƬ����
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255, AC_SRC_ALPHA });

}


//�����࣬������װ��������߼�������

/*class Button
{
public:
	Button()
	{
		region = 
	}
	~Button() = default;


private:
	RECT region;
	IMAGE img_idle;
	IMAGE img_hovered;
	IMAGE img_pushed;

	enum class Status
	{
		Idle = 0,
		Hovered = 0,
		Pushed
	};
};*/

class Animation
{
public:
	Animation(LPCTSTR path,int num, int interval)
	{
		interval_ms = interval;

		TCHAR path_file[256];
		for (size_t i = 0; i < num; i++)// forѭ������ͼƬ
		{
			_stprintf_s(path_file, path, i);

			IMAGE* frame = new IMAGE();
			loadimage(frame, path_file);
			frame_list.push_back(frame); //��ÿ���������ͼƬ������֡�б�
		}
	}

	~Animation()  //�����������������
	{
		for (size_t i = 0; i < frame_list.size(); i++)
		{
			delete frame_list[i];
		}
	}

	//�@��Play������ÿ���{��ֻ��Ⱦһ��
	void Play(int x, int y, int delta) 
	{
		/*�ڸ������ﶯ�����߼����ü�ʱ������
		���������ɿ�����Ϊ����������Ϊ����ˢ��
		�ٶȼӿ죬��������Խ�죬��ɲ��ȶ�*/

		timer += delta; //ÿ���{�ö�����һ���Α����Õr�g
		if (timer >= interval_ms)
		{
			idx_frame = (idx_frame + 1) % frame_list.size(); //��ȡ������֡����ѭ���仯
			timer = 0;
		}
		putimage_alpha(x, y, frame_list[idx_frame]);
	}

private: // һ��С�㣬 ���ṹ���Ա����Ҫʹ�õı�����������Ǵ��θ����ģ���һ��Ҫ�ڳ�Ա������д��
	int timer = 0; //ani ��ʱ��
	int idx_frame = 0; //����֡����
	int interval_ms = 0;
	std::vector<IMAGE*>frame_list;

};

//��Ҷ���ʵ����
Animation anim_left_player(_T("img/player_left_%d.png"), 3, 45);
Animation anim_right_player(_T("img/player_right_%d.png"), 3, 45);
//Ұ����ʵ����

// �ӵ���
class Bullet
{
public:
	POINT position{ 0, 0 };

public:
	Bullet() = default;
	~Bullet() = default;

	void Draw() const
	{
		setlinecolor(RGB(255, 155, 50));
		setfillcolor(RGB(200, 75, 10));
		fillcircle(position.x, position.y, RADIUS);
	}

private:
	const int RADIUS = 10;
};

class Player
{
public:
	
	const int FRAME_WIDTH = 80;//����߿�Ŀ��
	const int FRAME_HEIGHT = 80;//����߿�ĸ߶�
	Player()
	{
		loadimage(&img_shadow, _T("img/shadow_player.png"));
		anim_left = new Animation(_T("img/player_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/player_right_%d.png"), 6, 45);

	}

	~Player()
	{
		delete anim_left;
		delete anim_right;
	}

	void ProcessEvent(const ExMessage& msg) //������Ҳ�����Ϣ
	{
		switch (msg.message)
		{
		case WM_KEYDOWN:
		{

			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = true;
				break;
			case VK_DOWN:
				is_move_down = true;
				break;
			case VK_LEFT:
				is_move_left = true;
				break;
			case VK_RIGHT:
				is_move_right = true;
				break;

			}

		}
		break;

		case WM_KEYUP:
		{
			switch (msg.vkcode)
			{
			case VK_UP:
				is_move_up = false;
				break;
			case VK_DOWN:
				is_move_down = false;
				break;
			case VK_LEFT:
				is_move_left = false;
				break;
			case VK_RIGHT:
				is_move_right = false;
				break;
			}
		}
		break;
		}
	}

	void Move() //��������ƶ�
		{
		//��֤��ɫ��ͬһ�ٶ��ƶ��������б�����ٶȸ���Ĳ���
		//�������ԭ��
		    int dir_x = is_move_right - is_move_left;
			int dir_y = is_move_down - is_move_up;
			double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
			if (len_dir != 0)
			{
				double normalized_x = dir_x / len_dir;
				double normalized_y = dir_y / len_dir;
				player_pos.x += (int)(PLAYER_SPEED * normalized_x);
				player_pos.y += (int)(PLAYER_SPEED * normalized_y);
			}
			//ȷ������ڴ����ڲ�

			if (player_pos.x < 0)player_pos.x = 0;
			if (player_pos.y < 0)player_pos.y = 0;
			if (player_pos.x + FRAME_WIDTH > WINDOW_WIDTH) player_pos.x = WINDOW_WIDTH - FRAME_WIDTH;
			if (player_pos.y + FRAME_HEIGHT > WINDOW_HEIGHT) player_pos.y = WINDOW_HEIGHT - FRAME_HEIGHT;
		}

	void Draw(int delta) //ͼ����Ⱦ
	{
		int pos_shadow_x = player_pos.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		// ������ӰӦ�û��Ƶ�ˮƽλ��
		int pos_shadow_y = player_pos.y + FRAME_HEIGHT - 8;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		
		static bool facing_left = false;
		int dir_x = is_move_right - is_move_left;
		if (dir_x < 0)
		{
			facing_left = true;
		}
		else if (dir_x > 0)
		{
			facing_left = false;
		}

		if (facing_left)
			anim_left_player.Play(player_pos.x, player_pos.y, delta);
		else
			anim_right_player.Play(player_pos.x, player_pos.y, delta);
	
	}

	//�����ɫ���꣬
	const POINT& GetPosition() const
	{
		return player_pos;
	}

private:
	const int PLAYER_SPEED = 3;
	const int SHADOW_WIDTH = 32;
	const int WINDOW_HEIGHT = 720;
	const int WINDOW_WIDTH = 1280;
	POINT player_pos = { 500 , 500 };//���λ��   ��Ա������û���������
	 //��ͱ����һ���µ����⣬һ������Ҫ�õ���һ����������ֻ��ͨ�����κ������е��������Ա������ʵ����
	//��������������ֱ�Ӷ�����

private:
	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 500, 500 };
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
};


class Enemy
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 6, 45);
		anim_right = new Animation(_T("img/enemy_right_%d.png"), 6, 45);

		//�������ɱ߽�, ����ö��
		enum class SpawnEdge
		{
			Up = 0,
			Down,
			Left,
			Right
		};
		// �����������ڵ�ͼ�����λ��
		SpawnEdge edge = (SpawnEdge)(rand() % 4);
		switch (edge)
		/*
		�߼��� ����������ʱ��Ұ����±߽������ͼ�ϱ߽�
		��Ұ���ϱ߽������ͼ�±߽磬��������������
		*/
		{
		case SpawnEdge::Up:
			//position��Ұ������
			position.x = rand() % WINDOW_WIDTH;
			position.y = -FRAME_HEIGHT;
			break;
		case SpawnEdge::Down:
			position.x = rand() % WINDOW_WIDTH;
			position.y = WINDOW_HEIGHT;
			break;
		case SpawnEdge::Left:
			position.x = -FRAME_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		case SpawnEdge::Right:
			position.x = WINDOW_WIDTH;
			position.y = rand() % WINDOW_HEIGHT;
			break;
		}
	}
	
	
	bool CheckBulletCollision(const Bullet& bullet)
	{
		//���ӵ���ЧΪһ���㣬�жϵ��Ƿ��ڵ��˾�����
		bool is_overlap_x = bullet.position.x >= position.x && bullet.position.x <= position.x + FRAME_WIDTH;
		bool is_overlap_y = bullet.position.y >= position.y && bullet.position.y <= position.y + FRAME_HEIGHT;

		return is_overlap_x && is_overlap_y;
	}
	//�����Һ�Ұ���Ƿ���ײ
	bool CheckPlayerCollision(const Player& player)
	{
		//������������ʵ��
		const POINT& player_position = player.GetPosition();
		bool is_overlap_x = false;
		bool is_overlap_y = false;
		// �Ե������ĵ����Ҿ����ص���Ϊ��ײ���ı�׼
		POINT check_position = { position.x + FRAME_WIDTH / 2, position.y + FRAME_HEIGHT / 2 };
		//���������ĵ��Ƿ�����Ҿ�����
		is_overlap_x = check_position.x >= player_position.x && check_position.x <= player_position.x + FRAME_WIDTH;
		is_overlap_y = check_position.y >= player_position.y && check_position.y <= player_position.y + FRAME_HEIGHT;
	
		return is_overlap_x && is_overlap_y;
	}
	/*
	����ʶ�ظ�������const������Ԥ��������
	�����䴫��ʱ�����޸��Ե��´���
	*/
	void Move(const Player& player)
	{	//���ս�ɫxy����
		const POINT& player_position = player.GetPosition();
		int dir_x = player_position.x - position.x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			//��Player�����ƣ�ʹ���������ֽ���߼�
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			position.y += (int)(SPEED * normalized_y);
		}
	}

	void Draw(int delta)
	{
		const Player player;
		
		int pos_shadow_x = position.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		// ������ӰӦ�û��Ƶ�ˮƽλ��
		int pos_shadow_y = position.y + FRAME_HEIGHT - 35;
		putimage_alpha(pos_shadow_x, pos_shadow_y, &img_shadow);
		
		if (facing_left)
			anim_left->Play(position.x, position.y, delta);
		else
			anim_right->Play(position.x, position.y, delta);

	}

	~Enemy() //���������������ͷŶ�̬������ڴ�
	{
		delete anim_left;
		delete anim_right;
	}
	void Hurt()
	{
		//�������Ƿ����ˣ� ����ʹ��һ����ɱ
		alive = false;
	}

	bool CheckAlive()
	{
		//��ȡ��ǰ���˵Ĵ��״̬
		return alive;
	}


private:
	const int SPEED = 2;
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;
	const int WINDOW_HEIGHT = 720;
	const int WINDOW_WIDTH = 1280;
	

private:

	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0, 0 };
	bool facing_left = false;
	bool alive = true;
};



int idx_current_anim = 0;//�洢��ǰ֡

const int PLAYER_ANIM_NUM = 3;
const int PLAYER_SPEED = 3;

IMAGE img_player_left[PLAYER_ANIM_NUM];//����Ķ���֡
IMAGE img_player_right[PLAYER_ANIM_NUM];//����Ķ���֡

POINT player_pos = { 500 , 500 };//���λ��

#pragma comment(lib, "MSIMG32.LIB")


//�������ɺ���
void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)//�Ƚ���һ��vector����ָ�
{
	const int INTERVAL = 100;
	static int counter = 0;// ��̬���������{�ú���֮�g��׃��ֵʼ�K��׃���������ϴνY��
	if ((++counter) % INTERVAL == 0) //ÿ100*100/144ms��һֻ��
		enemy_list.push_back(new Enemy());
}

void UpdateBullets(std::vector<Bullet>& bullet_list, const Player& player)
{
	const double RADIAL_SPEED = 0.0045; //�����ٶ�
	const double TANGENT_SPEED = 0.0055; //�����ٶ�
	double radian_interval = 2 * 3.14159 / bullet_list.size();
	POINT player_position = player.GetPosition();
	double radius = 100 + 25 * sin(GetTickCount() * RADIAL_SPEED);
	//�����б��е�ÿһ���ӵ���������ҵ�ǰ��λ�������޸����ǵ�λ��
	for (size_t i = 0; i < bullet_list.size(); i++)
	{
		double radian = GetTickCount() * TANGENT_SPEED + radian_interval * i; //��ǰ�ӵ��Ļ���
		bullet_list[i].position.x = player_position.x + player.FRAME_WIDTH / 2 + (int)(radius * cos(radian));
		bullet_list[i].position.y = player_position.y + player.FRAME_HEIGHT / 2 + (int)(radius * sin(radian));

	}
}

void DrawPlayerScore(int score)
{
	static TCHAR text[64];
	_stprintf_s(text, _T("��ǰ��ҵ÷֣�%d"), score);

	settextstyle(50, 0, _T("Consolas"));
	setbkmode(TRANSPARENT);
	settextcolor(RGB(255, 85, 185));
	outtextxy(10, 10, text);
}


int main()
{
	initgraph(1280, 720);

	mciSendString(_T("open mus/bgm.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open mus/hit.wav alias hit"), NULL, 0, NULL);
	//��������

	mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
	bool running = true;

	int score = 0;
	Player player;
	ExMessage msg;
	IMAGE img_background;
	IMAGE img_shadow; //������Ӱ
	IMAGE img_enemy;
	std::vector<Enemy*> enemy_list;
	std::vector<Bullet> bullet_list(1);// �������������

	loadimage(&img_background, _T("img/background.png")); //���ر���
	
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
	
	

	loadimage(&img_shadow, _T("img/shadow_player.png"));
	
	BeginBatchDraw();
	
	//LoadAnimation();
	while (running)
	{
		DWORD start_time = GetTickCount();
		/*
		���ｫ����տ�ʼ���е�ʱ����¼��start����time�У�������ڽ��ж�̬��ʱ
		GetTickCount ��������ȡ����ǰ���е�ʱ��
		*/
		while (peekmessage(&msg))
		{
			player.ProcessEvent(msg);
		}

		player.Move();
		UpdateBullets(bullet_list, player);
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
		{
			enemy->Move(player);
		}
		
		
		// (��Ϸ�����ж�)��������vector���������μ��Ұ���Ƿ����������
		for (Enemy* enemy : enemy_list)
		{
			if (enemy->CheckPlayerCollision(player))
			{
				static TCHAR text[128];
				_stprintf_s(text, _T("���յ÷֣�%d !!!"), score);
				MessageBox(GetHWnd(),text, _T("��Ϸ����, �Ѿ�������"), MB_OK);
				running = false;
				break;
			}	

		}
		//����ӵ���Ұ�����ײ
		for (Enemy* enemy : enemy_list)
		{
			for (const Bullet& bullet : bullet_list)
			{
				if (enemy->CheckBulletCollision(bullet))
				{
					enemy->Hurt();
					mciSendString(_T("setaudio hit volume to 10000"), NULL, 0, NULL);
					mciSendString(_T("play hit repeat from 0"), NULL, 0, NULL);
					score++;
				}
			}
		}

		//�Ƴ�����ֵ����ĵ���
		for (size_t i = 0; i < enemy_list.size(); i++)
		{
			Enemy* enemy = enemy_list[i];
			if (!enemy->CheckAlive())
			{
				std::swap(enemy_list[i], enemy_list.back());
				enemy_list.pop_back();
				//����ɾ������Ԫ����������ϼ��� ��swap����ɾ��Ԫ�غͶ���Ԫ�ؽ���λ�ã���ɾ����Ԫ��
				delete enemy;
			}
		}
		
		cleardevice();
		
		putimage(0, 0, &img_background);
		player.Draw(1000 / 144);
		for (Enemy* enemy : enemy_list)
		{
			//ѭ������Draw����
			enemy->Draw(1000 / 144);
		}
		for (const Bullet& bullet : bullet_list)
		{
			bullet.Draw();
		}

		DrawPlayerScore(score);

		FlushBatchDraw();

		
		DWORD end_time = GetTickCount();
		DWORD delta_time = end_time - start_time;
		if (delta_time < 1000 / 144)
		{
			Sleep(1000 / 144 - delta_time);
		}
		/*
	��������ΪʲôҪ�Ӷ�̬��ʱ����ȷ����Ϸ��ȷ����֡�ʽ��и���
	����Ϸ�����У�ȷ����Ϸ����һ����֡�ʣ�ͨ����ÿ��֡�� FPS ��ʾ�������Ƿǳ���Ҫ�ġ���δ����������ʵ��֡�ʿ��ƣ�ȷ����Ϸ�ڲ�ͬ��Ӳ���϶������Ƶ��ٶ����У��Ӷ��ṩ��һ�µ���Ϸ���顣

	������˵����δ�������������¼������棺

	��Ϸ��ѭ�����ƣ� ����δ��������Ϸ��ѭ���У�ȷ��ÿ��ѭ��������ʱ�����ӽ�Ŀ��֡�ʡ��������Կ�����Ϸ�ĸ����ٶȣ�ʹ���ڲ�ͬ��Ӳ���ϱ������һ�µ����ܡ�

	��Ϸ����ģ�⣺ ����Ϸ�У�����ģ��ͨ����Ҫ�ڹ̶���ʱ�䲽���ڽ��м��㣬��ȷ������ģ����ȶ��Ժ�һ���ԡ���δ�����԰�����������ģ��ĸ���Ƶ�ʣ���������Ϸ����

	�������ƣ� ��Ϸ�еĶ���ͨ����Ҫ�Թ̶���֡�ʲ��ţ��Ա�֤�����Ժ�һ���ԡ�ͨ�����ƶ������µ�ʱ����������ȷ���������ŵ��ٶ��ȶ���������Ϸ��֡�ʱ���һ�¡�

	�ܵ���˵����δ�����԰�����Ϸ�����߿�����Ϸ�������ٶȺ�֡�ʣ��Ӷ��ṩ���õ���Ϸ���顣
		*/
	}	
	

	EndBatchDraw();

	return 0;
}