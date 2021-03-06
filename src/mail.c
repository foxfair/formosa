/**
 ** written by lthuang@cc.nsysu.edu.tw, 98/10
 ** Update: 99/10/15, lasehu@cc.nsysu.edu.tw
 **
 **/

#include "bbs.h"
#include "tsbbs.h"


/**************************************************************
 * 檢查個人信箱 Mail 數量
 **************************************************************/
BOOL check_mail_num(int opt)
{
	static int num_mails = -1;
	int num_delete = 0;
	static int maxkeepmail = -1;


	/* initialize */
	if (maxkeepmail == -1)
	{
		if (PERM_SYSOP == curuser.userlevel)
			maxkeepmail = 100 * SPEC_MAX_KEEP_MAIL;	/* lthuang */
/* By kmwang:20000529:For KHBBS 等級在100(含)以上者的信箱上限皆設為 200 */
#ifdef KHBBS
                else if (curuser.userlevel >= 100)
                        maxkeepmail = SPEC_MAX_KEEP_MAIL;
#else
                else if (PERM_BM == curuser.userlevel)
                        maxkeepmail = SPEC_MAX_KEEP_MAIL;
#endif
                else
                        maxkeepmail = MAX_KEEP_MAIL;
        }

/*
disable
	if (HAS_PERM(PERM_SYSOP))
		return FALSE;
*/
	if (num_mails == -1)
		num_mails = get_num_records(ufile_mail, FH_SIZE);
	if (num_mails > maxkeepmail && uinfo.ever_delete_mail)
	{
		int fd;

		if ((fd = open(ufile_mail, O_RDONLY)) > 0)
		{
			while (read(fd, &fhGol, sizeof(fhGol)) == sizeof(fhGol))
				if ((fhGol.accessed & FILE_DELE))
					num_delete++;
			close(fd);
		}
	}

	if (opt == 1 && num_delete)
	{
		msg("立刻刪除信件嗎? [n]: ");
		if (igetkey() == 'y')
		{
			pack_article(ufile_mail);
			num_mails -= num_delete;
		}
	}

	if (num_mails > maxkeepmail)
	{
		if (opt == 0)
		{
			msg("請先將您信箱內信件保留少於 %d 封!\n", maxkeepmail);
			getkey();
		}
		else if (opt == 1)
		{
			clear();
			prints(_msg_max_mail_warning, num_mails, maxkeepmail);
			outs("\n確定要離開嗎 (y/n) ? [n]: ");
			if (igetkey() == 'y')
				return FALSE;
		}
		return TRUE;
	}
	return FALSE;
}


struct word *mg_wlist = NULL;

/*
 * grouply deliver mail now, not support 'uuencode' here
 */
static int mail_group(char *fname, char *title)
{
	int msNew;
	struct word *wcur;

	if (!mg_wlist || check_mail_num(0))
		return -1;

	if ((msNew = CreateMailSocket()) < 0)
		return -1;

	msg("自己保存寄件備份 (y/n) ? [n]: ");
	if (igetkey() == 'y')
	{
		/* 寄件備份 */
		sprintf(genbuf, "[寄件備份] -- %s", title);
		/* TODO: check return vaule ? */
#ifndef IGNORE_CASE
                SendMail(msNew, fname, curuser.userid, curuser.userid, genbuf,
#else
                SendMail(msNew, fname, strcasecmp(curuser.fakeuserid,
curuser.userid)? curuser.userid:curuser.fakeuserid, curuser.userid, genbuf,
#endif
			curuser.ident);
	}

	/* delivery mail for the assigned list */
	for (wcur = mg_wlist; wcur; wcur = wcur->next)
	{
		if (!wcur->word || !wcur->word[0])
			continue;
		/* TODO: check return vaule ? */
		SendMail(msNew, fname, curuser.userid, wcur->word, title,
			curuser.ident);
	}

	CloseMailSocket(msNew);
	return 0;
}


/*ARGUSED*/
static int show_wlist(struct word *wtop)
{
	int x = 0, y = 3, cnt = 0;
	struct word *wcur;

	move(2, 0);
	clrtobot();

	for (wcur = wtop; wcur; wcur = wcur->next)
	{
		if (wcur->word && wcur->word[0])
		{
			move(y, x);
			prints("%d) %s", ++cnt, wcur->word);
			if (++y >= b_line)
			{
				y = 3;
				x += 16;
				if (x >= 80)
				{
					pressreturn();
					x = 0;
					move(y, x);
					clrtobot();
				}
			}
		}
	}
	if (cnt == 0)
	{
		move(y, x);
		outs(_msg_none);
	}
	return cnt;
}


/*
 * user can modify his list for grouply mail sending
 */
static int set_group()
{
	char strName[STRLEN];
	int num_send = 0;


	clear();
	prints(_msg_max_group, MAX_MAILGROUPS);

	for (;;)
	{
		/* show the list for grouply mail sending */
		show_wlist(mg_wlist);

		getdata(1, 0, _msg_ask_group_add, genbuf, 2, ECHONOSP | XLCASE);
		switch (genbuf[0])
		{
		case 'a':
			if (num_send >= MAX_MAILGROUPS)
			{
				prints(_msg_mail_group_max_prompt, MAX_MAILGROUPS);
				getkey();
				break;
			}

			if (getdata(2, 0, _msg_receiver, strName, sizeof(strName), ECHONOSP))
			{
#ifdef IGNORE_CASE
                                        strtolow(strName);
#endif
#if EMAIL_LIMIT
				if (curuser.ident != 7 && strchr(strName, '@'))
				{
					prints("\n%s", _msg_sorry_email);
					clrtoeol();
					getkey();
				}
				else
#endif
				if (strchr(strName, '@') || get_passwd(NULL, strName) > 0)
				{
					if (!cmp_wlist(mg_wlist, strName, strcmp))
					{
						add_wlist(&mg_wlist, strName, malloc_str);
						num_send++;
					}
				}
			}
			break;
		case 'f':
			{
				char *cbegin, *cend;

				malloc_array(&friend_cache, ufile_overrides);
				for (cbegin = friend_cache.ids;
					cbegin - friend_cache.ids < friend_cache.size; cbegin = cend + 1)
				{
					for (cend = cbegin; *cend; cend++)
						/* NULL STATEMENT */;
					if (!*cbegin)
						continue;

					if (num_send >= MAX_MAILGROUPS)
					{
						prints(_msg_mail_group_max_prompt, MAX_MAILGROUPS);
						getkey();
						break;
					}

					if (!cmp_wlist(mg_wlist, cbegin, strcmp))
					{
						add_wlist(&mg_wlist, cbegin, malloc_str);
						num_send++;
					}
				}
			}
			break;
		case 'd':
			if (getdata(2, 0, _msg_delete, genbuf, IDLEN, ECHONOSP))
			{
#ifdef IGNORE_CASE
                                strtolow(genbuf);
#endif
				if (cmpd_wlist(&mg_wlist, genbuf, strcmp, free) == 0)
					num_send--;
			}
			break;
		case 'q':
			return -1;
		case 'e':
		default:
			if (num_send == 0)
				return -1;
			clrtobot();
			return 0;
		}
	}
}


/*
 * grouply send mail, in cursor menu
 */
int m_group()
{
	int result = -1;
	char strTitle[STRLEN] = "";

#ifdef STRICT_IDENT
	if (curuser.ident != 7)
	{
		move(1, 0);
		outs(_msg_sorry_ident);
		pressreturn();
		return C_FULL;
	}
#endif

	if (set_group() != -1 && set_article_title(strTitle) != -1)
	{
		char fnameTmp[PATHLEN];

		sprintf(fnameTmp, "tmp/groupmail%05d", (int) getpid());
		unlink(fnameTmp);
		update_umode(SMAIL);
		if (!vedit(fnameTmp, strTitle, NULL))
			result = mail_group(fnameTmp, strTitle);
		unlink(fnameTmp);
	}
	free_wlist(&mg_wlist, free);
	if (result == -1)
		showmsg(_msg_fail);
	else
		showmsg(_msg_finish);

	return C_FULL;
}


/*
 * 寄信
 */
int m_send(int ent, FILEHEADER *finfo, char *direct)
{
	char strTo[STRLEN] = "", strTitle[STRLEN] = "";


	clear();
	/* fn_src: NULL, postpath: NULL */
	PrepareMail(NULL, strTo, strTitle);
	pressreturn();

	return C_FULL;
}


/*******************************************************************
 * 只讀新的信
 *******************************************************************/
int m_new()
{
	int fd, ent = 0;
	FILEHEADER fhn;

/* kmwang: 再讀新信時提醒 user 收信回去 */
#ifdef MAILWARN
	more(MAIL_WARN, TRUE);
#endif

	if ((fd = open(ufile_mail, O_RDONLY)) < 0)
		return C_FULL;

	in_mail = TRUE;	/* lthuang */

	do
	{
		update_umode(RMAIL);
		lseek(fd, 0, SEEK_SET);
		ent = 0;

		while (read(fd, &fhn, sizeof(fhGol)) == sizeof(fhGol))
		{
			ent++;
			if (fhn.accessed & FILE_READ)
				continue;

			clear();
			prints(_msg_m_new_read_prompt,
			       (fhn.owner[0] == '#') ? (fhn.owner + 1) : (fhn.owner),
			       fhn.title);

			switch (igetkey())
			{
			case 'n':
				continue;

			case 'q':
				close(fd);
				in_mail = FALSE;	/* lthuang */
				return C_FULL;

			default:
				break;
			}

			read_article(ent, &fhn, ufile_mail);
		}
		move(2, 0);
		clrtobot();
		outs(_msg_m_new_nomore);
		pressreturn();
	}
	while (check_mail_num(1));

	close(fd);
	in_mail = FALSE;	/* lthuang */
	return C_FULL;
}


/*
 * help for mail menu
 */
static int mail_help(int ent, FILEHEADER *finfo, char *direct)
{
	more(MAIL_HELP, TRUE);
	return C_FULL;
}


/*
 * show title in mail folder menu
 */
static void mail_title()
{
	title_func(_msg_mail_3, BBSTITLE);
	outs("\n\
(h)說明 (Ctrl-p)寫信 (</>)(a/A)搜尋 ([/])主題閱\讀 (U)查詢發信人 (T)整批刪除\n\
(d)刪除 (E)修改編輯 (m)寄出 (x)轉貼 (g)保留信件 (HOME)首篇 ($)末篇 ");
#if defined(NSYSUBBS1) || defined(KHBBS) /* sarek:09/23/2001:高市資教要求 */
	outs("(Ctrl-X)採證");
#endif
	outs("\n\
[7m   編號     發信人         日期    標題                                       [m");
}


#if defined(NSYSUBBS1) || defined(KHBBS) /* sarek:09/23/2001:高市資教要求 */
static int capture_mail(int ent, FILEHEADER *finfo, char *direct)
{
	char fnori[PATHLEN];


	msg("<<信件採證>: 你同意要將信件採證至 %s 板嗎 (y/n)? [n]: ",
		CAPTURE_BOARD);
	if (igetkey() != 'y')
	{
		msg(_msg_abort);
		getkey();
		return C_FOOT;
	}

	setdotfile(fnori, direct, finfo->filename);
	/*  post on board, postpath is NULL */
#ifdef	USE_THREADING	/* syhu */
/*
	if (PublishPost(fnori, curuser.userid, curuser.username, CAPTURE_BOARD,
		"[信件記錄]", curuser.ident, uinfo.from, FALSE, NULL, 0,-1,-1) == -1)
*/
	if (PublishPost(fnori, curuser.userid, uinfo.username, CAPTURE_BOARD,
		"[信件記錄]", curuser.ident, uinfo.from, FALSE, NULL, 0,-1,-1) == -1)
#else
/*
	if (PublishPost(fnori, curuser.userid, curuser.username, CAPTURE_BOARD,
		"[信件記錄]", curuser.ident, uinfo.from, FALSE, NULL, 0) == -1)
*/
	if (PublishPost(fnori, curuser.userid, uinfo.username, CAPTURE_BOARD,
		"[信件記錄]", curuser.ident, uinfo.from, FALSE, NULL, 0) == -1)
#endif
		showmsg(_msg_fail);
	else
		showmsg(_msg_finish);
	return C_FULL;
}
#endif


/*******************************************************************
 * Read Mail Menu 個人信箱  列表閱讀
 *******************************************************************/
int m_read()
{
	static int mail_ccur = 0;	/* the postion of cursor in mail folder */
	static struct one_key mail_comms[] =
	{
		{CTRL('P'), m_send},
		{'h', mail_help},
		{'r', read_article},
		{'m', mail_article},
		{'d', delete_article},
		{'t', tag_article},
		{'T', range_tag_article},
		{'E', edit_article},
		{'i', title_article},
		{'g', reserve_article},
		{'x', cross_article},
		{'U', rcmd_query},
		{'a', author_backward},
		{'A', author_forward},
		{'<', title_backward},
		{'?', title_backward},
		{'>', title_forward},
		{'/', title_forward},
		{'[', thread_backward},
		{']', thread_forward},
		{'-', resv_backward},
		{'+', resv_forward},
#if defined(NSYSUBBS1) || defined(KHBBS) /* sarek:09/23/2001:高市資教要求 */
		{CTRL('X'), capture_mail},
#endif
		{'\0', NULL}
	};


/* kmwang: 讀信時提醒 user 收信回去 */
#ifdef MAILWARN
        more(MAIL_WARN, TRUE);
#endif

	in_mail = TRUE;		/* lthuang */

	do
	{
		free_wlist(&artwtop, free);
		strcpy(tmpdir, ufile_mail);

		cursor_menu(4, 0, tmpdir, mail_comms, FH_SIZE, &mail_ccur,
			    mail_title, read_btitle,
			    read_entry, read_get, read_max, NULL, 1, FALSE, SCREEN_SIZE-4);
	}
	while (check_mail_num(1));

	in_mail = FALSE;

	return C_LOAD;
}
