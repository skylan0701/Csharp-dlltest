// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� DLLTEST_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// DLLRECV_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef DLLRECV_EXPORTS
#define DLLRECV_API __declspec(dllexport)
#else
#define DLLRECV_API __declspec(dllimport)
#endif

// �����Ǵ� dlltest.dll ������
class DLLRECV_API Cdllrecv {
public:
	Cdllrecv(void);
	// TODO:  �ڴ�������ķ�����
};
EXTERN_C DLLRECV_API int _stdcall my_recv(void);
