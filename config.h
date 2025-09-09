/* See LICENSE file for copyright and license details. */

#include <X11/XF86keysym.h>
#include <memory.h>

/* appearance */
static const unsigned int borderpx  = 2;        /* border pixel of windows */
static const unsigned int gappx     = 0;        /* gaps between windows */
static const unsigned int snap      = 32; /* light */       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int user_bh            = 0;        /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const char *fonts[]          = { "minecraft :size=16" };
static const char dmenufont[]       = "Monocraft :size=16";

static const char gruvboxred[] = "#fb4934";
static const char gruvboxgreen[] = "#b8bb26";
static const char gruvboxyellow[] = "#fabd2f";
static const char gruvboxblue[] = "#83a598";
static const char gruvboxpurple[] = "#d3896b";

static const char gruvboxfg[] = "#ebdbb2";
static const char gruvboxbg1[] = "#3c3836";
static const char gruvboxbg2[] = "#504945";
static const char gruvboxbg3[] = "#665c54";
static const char gruvboxbg4[] = "#7c6f64";

static const char accent[] = "#b8bb26";

static const char *colors[][SchemeN][3] = {
		/*               fg         bg         border   */
	{
		[SchemeNorm] = { gruvboxfg, gruvboxbg1, gruvboxfg},
		[SchemeSel]  = { gruvboxfg, gruvboxbg1,  accent},
	},
	{
		[SchemeNorm] = { gruvboxfg, gruvboxbg1, gruvboxfg},
		[SchemeSel]  = { gruvboxfg, gruvboxfg,  accent},
	}
};


/* tagging */
static const char *tags[] = { "A", "S", "D", "F", "J", "K", "L", ";" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	{ "Gimp",     NULL,       NULL,       0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle}
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

/* applications */
#define WEB_BROWSER "zen"
#define TERMINAL_EMULATOR "alacritty"
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", gruvboxbg1, "-nf", gruvboxfg, "-sb", accent, "-sf", gruvboxfg, NULL };
static const char *termcmd[]  = { TERMINAL_EMULATOR, NULL };
static const char *browsercmd[]  = { WEB_BROWSER,  NULL };
static const char *youtubecmd[]  = { WEB_BROWSER, "--new-window", "https://youtube.com", NULL };
static const char *discordcmd[]  = { "discord", NULL };
static const char *claudecmd[]  = { WEB_BROWSER, "--new-window", "https://claude.ai/new", NULL };
static const char *githubcmd[]  = { WEB_BROWSER, "--new-window", "https://github.com/EggbertFluffle", NULL };
static const char *schoolemailcmd[]  = { WEB_BROWSER, "--new-window", "https://outlook.office365.com/mail/", NULL };
static const char *spotifycmd[]  = { WEB_BROWSER, "--new-window", "https://open.spotify.com", NULL };
static const char *emacscmd[]  = { "emacsclient", "-r", NULL };
static const char *processmonitorcmd[] = { TERMINAL_EMULATOR, "-e", "btop", NULL };
static const char *bluetuithcmd[] = { TERMINAL_EMULATOR, "-e", "bluetuith", NULL };
static const char *wirelesscontrolscmd[] = { "iwgtk", NULL };
static const char *audiocontrolscmd[] = { "pavucontrol", NULL };
// static const char *screenshotcmd[] = { "scrot", "-s", "-f", NULL };
// Can you take the stirng below and seperate it into an array of string for every space? like the one above
// static const char *screenshotcmd[] = { "scrot -s -e 'xclip -selection clipboard -target image/png -i $f'", NULL};
static const char *screenshotcmd[] = { "scrot", "-s", "-e", "'xclip -selection clipboard -target image/png -i $f'", NULL};
static const char *boomerzoomcmd[] = { "boomer", NULL };

/* volume */
static const char *volupcmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+3%", NULL };
static const char *voldowncmd[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-3%", NULL };
// static const char *mutevolume[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-3%", NULL };

/* brightness */
static const char *brightnessupcmd[] = { "brightnessctl", "-d", "intel_backlight", "set", "2%+", NULL };
static const char *brightnessdowncmd[] = { "brightnessctl", "-d", "intel_backlight", "set", "2%-", NULL };

/* power */
static const char *sleepcmd[] = { "sudo", "zzz", NULL };
static const char *poweroffcmd[] = { "sudo", "shutdown", "-h", "now", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_i,      spawn,          {.v = browsercmd } },
	{ MODKEY|ShiftMask,             XK_y,      spawn,          {.v = youtubecmd } },
	{ MODKEY|ShiftMask,             XK_d,      spawn,          {.v = discordcmd } },
	{ MODKEY|ShiftMask,             XK_g,      spawn,          {.v = githubcmd } },
	{ MODKEY|ShiftMask,             XK_m,      spawn,          {.v = schoolemailcmd } },
	{ MODKEY|ShiftMask,             XK_t,      spawn,          {.v = spotifycmd } },
	{ MODKEY|ShiftMask,             XK_p,      spawn,          {.v = processmonitorcmd } },
	{ MODKEY|ShiftMask,             XK_b,      spawn,          {.v = bluetuithcmd } },
	{ MODKEY|ShiftMask,             XK_l,      spawn,          {.v = claudecmd } },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          {.v = screenshotcmd } },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          {.v = wirelesscontrolscmd } },
	{ MODKEY|ShiftMask,             XK_a,      spawn,          {.v = audiocontrolscmd} },
	{ MODKEY|ShiftMask,             XK_e,      spawn,          {.v = emacscmd } },
	{ MODKEY|ShiftMask,             XK_z,      spawn,          {.v = boomerzoomcmd } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.02} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.02} },
	{ MODKEY,                       XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefullscr,  {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_space,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	TAGKEYS(                        XK_a,                      0)
	TAGKEYS(                        XK_s,                      1)
	TAGKEYS(                        XK_d,                      2)
	TAGKEYS(                        XK_f,                      3)
	TAGKEYS(                        XK_j,                      4)
	TAGKEYS(                        XK_k,                      5)
	TAGKEYS(                        XK_l,                      6)
	TAGKEYS(                        XK_semicolon,              7)
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
	{ MODKEY|ShiftMask,  XK_bracketright,      spawn,          {.v = volupcmd } },
	{ MODKEY|ShiftMask,   XK_bracketleft,      spawn,          {.v = voldowncmd } },
//  { MODKEY|ShiftMask,     XK_backslash,      spawn,          {.v = mutecmd } },
	{ MODKEY,            XK_bracketright,      spawn,          {.v = brightnessupcmd } },
	{ MODKEY,             XK_bracketleft,      spawn,          {.v = brightnessdowncmd } },
	{ MODKEY,             XF86XK_PowerOff,     spawn,          {.v = sleepcmd } },
	{ MODKEY|ShiftMask,   XF86XK_PowerOff,     spawn,          {.v = poweroffcmd } }
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
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
