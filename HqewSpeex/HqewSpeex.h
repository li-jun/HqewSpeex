// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� HQEWSPEEX_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// HQEWSPEEX_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef HQEWSPEEX_EXPORTS
#define HQEWSPEEX_API __declspec(dllexport)
#else
#define HQEWSPEEX_API __declspec(dllimport)
#endif

extern HQEWSPEEX_API int nHqewSpeex;

HQEWSPEEX_API int fnHqewSpeex(void);
 
