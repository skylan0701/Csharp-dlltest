// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� DLLTEST_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// DLLTEST_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef DLLTEST_EXPORTS
#define DLLTEST_API __declspec(dllexport)
#else
#define DLLTEST_API __declspec(dllimport)
#endif

// �����Ǵ� dlltest.dll ������
class DLLTEST_API Cdlltest {
public:
	Cdlltest(void);
	// TODO:  �ڴ�������ķ�����
};

EXTERN_C DLLTEST_API int _stdcall my_send(char *filename);
EXTERN_C DLLTEST_API int _stdcall my_recv(long &flag);
EXTERN_C DLLTEST_API int _stdcall luyin(long &flag);
