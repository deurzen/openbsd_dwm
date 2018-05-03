/* See LICENSE file for copyright and license details. */
#include "extensions/cyclenonmaster.c"
#include "extensions/gaplessgrid.c"

/* appearance */
static const char *fonts[] = {
	"terminus:size=8"
};

static const char dmenufont[]            = "terminus:size=8";
static const char normbordercolor[]      = "#404040";
static const char normbgcolor[]          = "#202020";
static const char normfgcolor[]          = "#c0c0c0";
static const char selbordercolor[]       = "#c0c0c0";
static const char selbgcolor[]           = "#404040";
static const char selfgcolor[]           = "#f0f0f0";
static const int showbar                 = 1;
static const int topbar                  = 1;
static const int focusonwheel            = 0;
static const int showsystray             = 1;
static const int systraypinningfailfirst = 1;
static const unsigned borderpx           = 1;
static const unsigned snap               = 0;
static const unsigned gappx              = 3;
static const unsigned systraypinning     = 0;
static const unsigned systrayspacing     = 2;


/* tagging */
static const char *tags[] = { "1:main", "2:web", "3:term", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "st-float",    NULL,       NULL,       0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
	{ "|||",      deck },
	{ "|M|",      centeredmaster },
	{ ">M>",      centeredfloatingmaster },
	{ "|#|",      gaplessgrid },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static       char      dmenumon[2]  = "0"; /* component of dmenucmd, manipulated in spawn() */
// program commands
static const char      *dmenucmd[]  = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", normbgcolor, "-nf", normfgcolor, "-sb", selbgcolor, "-sf", selfgcolor, NULL };
static const char       *rofipass[] = { "rofi-pass", NULL };
//static const char        *termcmd[] = { "st", NULL };
//static const char        *tmuxcmd[] = { "st", "-e", "tmux", NULL };
//static const char        *firefox[] = { "firefox", NULL };
//static const char    *qutebrowser[] = { "qutebrowser", NULL };
//static const char  *qutebrowseror[] = { "qutebrowser", "-R", NULL };
//static const char    *termneomutt[] = { "st", "-e", "zsh", "-i", "-c", "'neomutt'", NULL };
//static const char     *termranger[] = { "w3mterm", "-e", "zsh", "-i", "-c", "'ranger'", NULL };
//static const char  *floattermrngr[] = { "w3mterm", "-g", "120x34+203+129", "-c", "st-float", "-e", "zsh", "-i", "-c", "'ranger'", NULL };
//static const char      *termsncli[] = { "st", "-e", "zsh", "-i", "-c", "'sncli'", NULL };
//static const char *floattermsncli[] = { "st", "-g", "120x34+203+129", "-c", "st-float", "-e", "zsh", "-i", "-c", "'sncli'", NULL };
//static const char        *termrtv[] = { "st", "-e", "zsh", "-i", "-c", "'rtv'", NULL };
static const char        *termcmd[] = { "xterm", NULL };
static const char        *tmuxcmd[] = { "xterm", "-e", "tmux", NULL };
static const char        *firefox[] = { "firefox", NULL };
static const char    *qutebrowser[] = { "qutebrowser", NULL };
static const char  *qutebrowseror[] = { "qutebrowser", "-R", NULL };
static const char    *termneomutt[] = { "xterm", "-e", "zsh", "-i", "-c", "'neomutt'", NULL };
static const char     *termranger[] = { "w3mterm", "-e", "zsh", "-i", "-c", "'ranger'", NULL };
static const char  *floattermrngr[] = { "w3mterm", "-g", "120x34+203+129", "-c", "xterm-float", "-e", "zsh", "-i", "-c", "'ranger'", NULL };
static const char      *termsncli[] = { "xterm", "-e", "zsh", "-i", "-c", "'sncli'", NULL };
static const char *floattermsncli[] = { "xterm", "-g", "120x34+203+129", "-c", "xterm-float", "-e", "zsh", "-i", "-c", "'sncli'", NULL };
static const char        *termrtv[] = { "xterm", "-e", "zsh", "-i", "-c", "'rtv'", NULL };
static const char   *nontransterm[] = { "w3mterm", NULL };
static const char     *lockscreen[] = { "7lock", NULL };
static const char          *grabc[] = { "grabc", NULL };

// mpc commands
static const char      *mpctoggle[] = { "mpc", "toggle", NULL };
static const char        *mpcnext[] = { "mpc", "next", NULL };
static const char        *mpcprev[] = { "mpc", "prev", NULL };
static const char        *mpcstop[] = { "mpc", "stop", NULL };
static const char      *mpcrandom[] = { "mpc", "random", NULL };
static const char      *mpcsingle[] = { "mpc", "single", NULL };

// multimedia binds
static const char       *volumeup[] = { "pactl", "set-sink-volume", "0", "+5%", NULL };
static const char     *volumedown[] = { "pactl", "set-sink-volume", "0", "-5%", NULL };
static const char     *volumemute[] = { "pactl", "set-sink-mute", "0", "toggle", NULL };
static const char       *brightup[] = { "light", "-A", "15", NULL };
static const char     *brightdown[] = { "light", "-U", "15", NULL };
static const char        *lightup[] = { "light", "-A", "5", NULL };
static const char      *lightdown[] = { "light", "-U", "5", NULL };
static const char  *selscreenshot[] = { "bash", "-c", "maim -s $(date +/home/deurzen/screenshots/scrots/SS_%Y-%h-%d_%H-%M-%S.png)", NULL };
static const char     *screenshot[] = { "bash", "-c", "maim $(date +/home/deurzen/screenshots/scrots/SS_%Y-%h-%d_%H-%M-%S.png)", NULL };

// general management commands
static const char scratchpadname[] = "scratchpad";
static const char *scratchpadcmd[] = { "scratchpadterm", "-g", "140x38", NULL };


static Key keys[] = {
	/* modifier                     key        function        argument */
// program binds
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_p,      spawn,          {.v = rofipass} },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ControlMask,           XK_Return, spawn,          {.v = tmuxcmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = nontransterm } },
	{ MODKEY,                       XK_grave,  togglescratch,  {.v = scratchpadcmd } },
	{ MODKEY|ShiftMask,             XK_g,      spawn,          {.v = grabc} },
	{ MODKEY,                       XK_e,      spawn,          {.v = termneomutt} },
	{ MODKEY,                       XK_q,      spawn,          {.v = firefox} },
	{ MODKEY|ShiftMask,             XK_q,      spawn,          {.v = qutebrowser} },
	{ MODKEY,                       XK_f,      spawn,          {.v = termranger} },
	{ MODKEY|ShiftMask,             XK_f,      spawn,          {.v = floattermrngr} },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          {.v = termsncli} },
	{ MODKEY,                       XK_s,      spawn,          {.v = floattermsncli} },
	{ MODKEY,                       XK_w,      spawn,          {.v = termrtv} },
	{ MODKEY|ShiftMask,             XK_Delete, spawn,          {.v = lockscreen} },
// mpc binds
	{ MODKEY|Mod1Mask,              XK_p,      spawn,          {.v = mpctoggle} },
	{ MODKEY|Mod1Mask,              XK_j,      spawn,          {.v = mpcnext} },
	{ MODKEY|Mod1Mask,              XK_k,      spawn,          {.v = mpcprev} },
	{ MODKEY|Mod1Mask,              XK_BackSpace,spawn,        {.v = mpcstop} },
	{ MODKEY|Mod1Mask,              XK_r,      spawn,          {.v = mpcrandom} },
	{ MODKEY|Mod1Mask,              XK_s,      spawn,          {.v = mpcsingle} },
// multimedia binds
	{ 0,                            0x1008ff13,spawn,          {.v = volumeup} },
	{ 0,                            0x1008ff11,spawn,          {.v = volumedown} },
	{ 0,                            0x1008ff12,spawn,          {.v = volumemute} },
	{ 0,                            0x1008ff02,spawn,          {.v = brightup} },
	{ 0,                            0x1008ff03,spawn,          {.v = brightdown} },
	{ ShiftMask,                    0x1008ff02,spawn,          {.v = lightup} },
	{ ShiftMask,                    0x1008ff03,spawn,          {.v = lightdown} },
	{ 0,                            XK_Print,  spawn,          {.v = selscreenshot} },
	{ ShiftMask,                    XK_Print,  spawn,          {.v = screenshot} },
// layout binds
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      rotatestack,    {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      rotatestack,    {.i = -1 } },
	{ MODKEY|ControlMask,           XK_j,      cyclenonmaster, {.i = +1 } },
	{ MODKEY|ControlMask,           XK_k,      cyclenonmaster, {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_BackSpace,zoom,         {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_b,      togglebar,      {0} },
	{ MODKEY|Mod1Mask|ShiftMask,    XK_x,      quit,           {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask|ControlMask, XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_d,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY,                       XK_u,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_o,      setlayout,      {.v = &layouts[5]} },
	{ MODKEY,                       XK_g,      setlayout,      {.v = &layouts[6]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY|ControlMask,           XK_BackSpace,killclient,   {0} },
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
};

/* button definitions */
/* click can be ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

