/*

������־��Ŀǰû�а���Ҳ��ݵؽ�ɫ��class��װ�ã�������ӵ��������߼������⣬
Enemy���е�GetPosition����ûд


��Ŀ���ƣ�Warrior fights bugs
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

class Enemy 
{
public:
	Enemy()
	{
		loadimage(&img_shadow, _T("img/shadow_enemy.png"));
		anim_left = new Animation(_T("img/enemy_left_%d.png"), 3, 45);
		anim_right = new Animation(_T("img/enemy_left_%d.png"), 3, 45);

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
		const POINT& player_postion = player.GetPosition();
		int dir_x = player_position.x - position, x;
		int dir_y = player_position.y - position.y;
		double len_dir = sqrt(dir_x * dir_x + dir_y * dir_y);
		if (len_dir != 0)
		{
			double normalized_x = dir_x / len_dir;
			double normalized_y = dir_y / len_dir;
			position.x += (int)(SPEED * normalized_x);
			postion.y += (int)(SPEED * normalized_y);
		}
	}
	
	~Enemy() //���������������ͷŶ�̬������ڴ�
	{
		delete anmi_left;
		delete anmi_right;
	}

private:
	const int SPEED = 2;
	const int FREAME_WIDTH = 80;
	const int FRAME_HEIGHT = 80;
	const int SHADOW_WIDTH = 48;
}

private:

	IMAGE img_shadow;
	Animation* anmi_left;
	Animation* anmi_right;
	POINT postion = { 0, 0 };
	bool facing_left = false;




int idx_current_anim = 0;//�洢��ǰ֡

const int PLAYER_ANIM_NUM = 3;
const int PLAYER_SPEED = 3;

IMAGE img_player_left[PLAYER_ANIM_NUM];//����Ķ���֡
IMAGE img_player_right[PLAYER_ANIM_NUM];//����Ķ���֡
POINT player_pos = { 500 , 500 };


#pragma comment(lib, "MSIMG32.LIB")

//���û��Ҫ���ֻ�Ƿ�װ��һ������ɫ͸�����ֵļ���ͼƬ����
inline void putimage_alpha(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255, AC_SRC_ALPHA });

}

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

int main()
{
	initgraph(1280, 720);
	LoadAnimation();

	bool running = true;
	bool turnning = true;

	ExMessage msg;
	IMAGE img_background;

	//����������߼���Ҫ��
	IMAGE img_enemy;

	bool is_move_up = false;
	bool is_move_down = false;
	bool is_move_left = false;
	bool is_move_right = false;


	loadimage(&img_background, _T("img/background.png")); //���ر���
	loadimage(&img_enemy, _T("img/enemy_right_1.png"));
	BeginBatchDraw();

	while (running)
	{
		DWORD start_time = GetTickCount();
		/*
		���ｫ����տ�ʼ���е�ʱ����¼��start����time�У�������ڽ��ж�̬��ʱ
		GetTickCount ��������ȡ����ǰ���е�ʱ��
		*/
		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN)
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
					turnning = false;
					break;
				case VK_RIGHT:
					is_move_right = true;
					turnning = true;
					break;
				}
			}
			
			if (msg.message == WM_KEYUP)
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
		}

		if (is_move_up)
		{
			player_pos.y -= PLAYER_SPEED;
		}
		else if (is_move_down)
		{
			player_pos.y += PLAYER_SPEED;
		}
		else if (is_move_left)
		{
			player_pos.x -= PLAYER_SPEED;
		}
		else if (is_move_right)
		{
			player_pos.x += PLAYER_SPEED;
		}

		static int counter = 0; //��¼�����˼�����Ϸ֡
		if (++counter % 5 == 0)// ÿ�����Ϸ֡�л�һ������֡
		{
			idx_current_anim++;
		}
		//ʹ����ѭ�����ţ�����
		idx_current_anim = idx_current_anim % PLAYER_ANIM_NUM;
		cleardevice();
		
		putimage(0, 0, &img_background);
		//������Ⱦ�߼�ֻ����ʱ��
		putimage_alpha(100, 100, &img_enemy);
		putimage_alpha(200, 250, &img_enemy);
		putimage_alpha(150, 200, &img_enemy);
		putimage_alpha(300, 60, &img_enemy);
		
		//�ж���ɫ���ƶ�����
		if (turnning == false)
		{
			putimage_alpha(player_pos.x, player_pos.y, &img_player_left[idx_current_anim]);
		}
		else if (turnning == true)
		{
			putimage_alpha(player_pos.x, player_pos.y, &img_player_right[idx_current_anim]);
		}
		
		//��Ⱦ����

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