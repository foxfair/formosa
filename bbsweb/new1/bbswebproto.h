#include "struct.h"
#define _BBS_PROTO_H_
#if __STDC__ || defined(__cplusplus)
#define P_(s) s
#else
#define P_(s) ()
#endif

/* article.c */
void write_article_line P_((FILE *fp, char *data, int type));
void write_article_body P_((FILE *fp, char *data, int type));
int PostArticle P_((char *pbuf, BOARDHEADER *board, POST_FILE *pf));
int EditArticle P_((char *pbuf, BOARDHEADER *board, POST_FILE *pf));
int DeleteArticle P_((char *pbuf, BOARDHEADER *board, POST_FILE *pf));
int ShowArticle P_((char *filename, BOOL body_only, BOOL process));
/* bbsweb.c */
void DoTagCommand P_((char *type, char *tag));
int WebLoginCheck P_((void));
int DoGetRequest P_((REQUEST_REC *rc, BOARDHEADER *board, POST_FILE *pf));
int DoPostRequest P_((REQUEST_REC *r, BOARDHEADER *board, POST_FILE *pf));
void WebMain P_((int child_num));
/* cache.c */
void init_cache P_((void));
unsigned int hash_string P_((const void *data));
int CacheState P_((char *filename, SKIN_FILE *sf));
int test_weight P_((time_t now, time_t age, int hit));
int select_file_cache_slot P_((char *file));
int select_html_cache_slot P_((char *file));
int do_cache_file P_((char *file));
int do_cache_html P_((char *file));
/* file.c */
int ShowFile P_((SKIN_FILE *sf));
int GetPostInfo P_((BOARDHEADER *board, POST_FILE *pf));
BOOL GetFileInfo P_((SKIN_FILE *sf));
int GetFileMimeType P_((char *filename));
void SetExpire P_((SKIN_FILE *sf));
/* http.c */
void base64_decode_str P_((char *src));
int GetHttpRequestType P_((char *request));
BOOL client_reload P_((char *pragma));
int GetMimeType P_((char *ext));
void ShowHttpHeader P_((REQUEST_REC *r, SKIN_FILE *sf, POST_FILE *pf));
int ParseHttpHeader P_((REQUEST_REC *r, SERVER_REC *s));
/* log.c */
void weblog P_((char *msg, char *logfile));
void weblog_line P_((FILE *fp, char *fmt, ...));
void OpenLogFile P_((SERVER_REC *server));
void CloseLogFile P_((SERVER_REC *server));
void FlushLogFile P_((SERVER_REC *server));
/* main.c */
void write_pidfile P_((pid_t pid, int port));
int unlink_pidfile P_((int port));
void usage P_((char *prog));
int main P_((int argc, char *argv[]));
/* post.c */
void ShowPost P_((char *tag, BOARDHEADER *board, POST_FILE *pf));
int ShowPostList P_((char *tag, BOARDHEADER *board, POST_FILE *pf));
/* signal.c */
void init_signals P_((void));
void shutdown_server P_((int sig));
void sig_segv P_((int sig));
void reaper P_((int sig));
void timeout_check P_((int s));
/* sysinfo.c */
void ShowServerInfo P_((char *tag, SERVER_REC *s, REQUEST_REC *r, FILE_SHM *file_shm, HTML_SHM *html_shm));
/* uri.c */
int ParseURI P_((REQUEST_REC *r, BOARDHEADER *CurBList, POST_FILE *pf));
/* user.c */
void UpdateUserRec P_((int action, USEREC *curuser, BOARDHEADER *board));
int CheckUserPassword P_((char *username, char *password));
void ShowUser P_((char *tag, USEREC *curuser));
void ShowUserList P_((char *tag, POST_FILE *pf));
int NewUser P_((char *pbuf, USEREC *curuser));
int id_num_check P_((char *num));
int check_cname P_((unsigned char name[]));
int DoUserIdent P_((char *pbuf, USEREC *curuser));
int UpdateUserData P_((char *pbuf, USEREC *curuser));
int UpdateUserSign P_((char *pbuf, USEREC *curuser));
int UpdateUserPlan P_((char *pbuf, USEREC *curuser));
int UpdateUserFriend P_((char *pbuf, USEREC *curuser));
/* util_date.c */
int checkmask P_((const char *data, const char *mask));
time_t tm2sec P_((const struct tm *t));
time_t parseHTTPdate P_((const char *date));
/* webboard.c */
void ShowBoard P_((char *tag, BOARDHEADER *board, POST_FILE *pf));
void ShowBoardList P_((char *tag, POST_FILE *pf));
int CheckBoardPerm P_((BOARDHEADER *board, USEREC *user));
int ModifyAccessList P_((char *pbuf, BOARDHEADER *board));
int ModifySkin P_((char *pbuf, BOARDHEADER *board, POST_FILE *pf));
/* webmail.c */
int isExceedMailLimit P_((USEREC *user));
void ShowMail P_((char *tag));
int MailCheck P_((char *address));
int ForwardArticle P_((char *pbuf, BOARDHEADER *board, POST_FILE *pf));
/* weblib.c */
BOOL isPost P_((const char *para));
BOOL isList P_((const char *para, int *start, int *end));
BOOL isBadURI P_((const char *uri));
void strip_html P_((char *fname));
void find_list_range P_((int *start, int *end, int current, int page_size, int max_size));
void souts P_((char *str, int maxlen));
char *GetBBSTag P_((char *type, char *tag, char *data));
char *GetFormBody P_((int content_length, char *WEBBBS_ERROR_MESSAGE));
int build_format_array P_((FORMAT_ARRAY *format_array, const char *data, char *head, char *tail, int max_tag_section));
void Convert P_((char *from, char *to));
void Convert1 P_((char *from, char *to));
BOOL GetPara2 P_((char *Para, const char *Name, const char *Data, int len, const char *def));
void GetPara3 P_((char *Para, char *Name, char *Data, int len, char *def));
void mk_timestr1 P_((char *str, time_t when));
void mk_timestr2 P_((char *str, time_t when));
int invalid_fileheader P_((FILEHEADER *fh));
int friend_list_set P_((char *file, char *pbuf, char *file_desc));

#undef P_
