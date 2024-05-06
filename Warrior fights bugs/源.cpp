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

//���û��Ҫ���ֻ�Ƿ�װ��һ������ɫ͸�����ֵļ���ͼƬ����
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255, AC_SRC_ALPHA });

}


//�����࣬������װ��������߼�������

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

//����ʵ����
Animation anim_left_player(_T("img/player_left_%d.png"), 3, 45);
Animation anim_right_player(_T("img/player_right_%d.png"), 3, 45);


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
		return position;
	}

private:
	const int PLAYER_SPEED = 3;
	const int FRAME_WIDTH = 80;//����߿�Ŀ��
	const int FRAME_HEIGHT = 80;//����߿�ĸ߶�
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
	
	
	bool CheckBulletCollision(const Bullet& Bullet)
	{
		return false;
	}

	bool CheckPlayerCollision(const Player& player)
	{
		return false;
	}
	/*
	����ʶ�ظ�������const������Ԥ��������
	�����䴫��ʱ�����޸��Ե��´���
	*/
	void Move(const Player& player)
	{
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
		int pos_shadow_x = player_pos.x + (FRAME_WIDTH / 2 - SHADOW_WIDTH / 2);
		// ������ӰӦ�û��Ƶ�ˮƽλ��
		int pos_shadow_y = player_pos.y + FRAME_HEIGHT - 35;
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

private:
	const int SPEED = 2;
	const int FRAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;
	const int WINDOW_HEIGHT = 720;
	const int WINDOW_WIDTH = 1280;
	POINT player_pos = { 500 , 500 };//���λ��

private:

	IMAGE img_shadow;
	Animation* anim_left;
	Animation* anim_right;
	POINT position = { 0, 0 };
	bool facing_left = false;
};



int idx_current_anim = 0;//�洢��ǰ֡

const int PLAYER_ANIM_NUM = 3;
const int PLAYER_SPEED = 3;

IMAGE img_player_left[PLAYER_ANIM_NUM];//����Ķ���֡
IMAGE img_player_right[PLAYER_ANIM_NUM];//����Ķ���֡

POINT player_pos = { 500 , 500 };//���λ��

#pragma comment(lib, "MSIMG32.LIB")


void LoadAnimation() //���ｫ��Ҫ�Ķ���֡����
{
	
	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_left_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_left[i], path.c_str());
	}

	for (size_t i = 0; i < PLAYER_ANIM_NUM; i++)
	{
		std::wstring path = L"img/player_right_" + std::to_wstring(i) + L".png";
		loadimage(&img_player_right[i], path.c_str());
	}
}


//�������ɺ���
void TryGenerateEnemy(std::vector<Enemy*>& enemy_list)//�Ƚ���һ��vector����ָ�
{
	const int INTERVAL = 100;
	static int counter = 0;// �o�B׃�������{�ú���֮�g��׃��ֵʼ�K��׃���������ϴνY��
	if ((++counter) % INTERVAL == 0) //����һ���b�i�ͣ���enemy�M��һ�Ό�����
		enemy_list.push_back(new Enemy());
}


int main()
{
	initgraph(1280, 720);

	bool running = true;

	Player player;
	ExMessage msg;
	IMAGE img_background;
	IMAGE img_shadow; //������Ӱ
	IMAGE img_enemy;
	std::vector<Enemy*> enemy_list;

	loadimage(&img_background, _T("img/background.png")); //���ر���
	
	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;
	
	

	loadimage(&img_shadow, _T("img/shadow_player.png"));
	
	BeginBatchDraw();
	
	LoadAnimation();
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
		TryGenerateEnemy(enemy_list);
		for (Enemy* enemy : enemy_list)
		{
			enemy->Move(player);
		}

		cleardevice();
		
		putimage(0, 0, &img_background);
		player.Draw(1000 / 144);
		for (Enemy* enemy : enemy_list)
		{
			//ѭ������Draw����
			enemy->Draw(1000 / 144);
		}

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