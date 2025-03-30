#include "config.h"
#include "defines.h"
#include "utils.h"

MinUISettings settings = {0};

// deprecated
uint32_t THEME_COLOR1_255;
uint32_t THEME_COLOR2_255;
uint32_t THEME_COLOR3_255;
uint32_t THEME_COLOR4_255;
uint32_t THEME_COLOR5_255;
uint32_t THEME_COLOR6_255;

static inline uint32_t HexToUint32_unmapped(const char *hexColor) {
    // Convert the hex string to an unsigned long
    uint32_t value = (uint32_t)strtoul(hexColor, NULL, 16);
    return value;
}

void CFG_defaults(MinUISettings *cfg)
{
    if (!cfg)
        return;

    MinUISettings defaults = {
        .font = 1, // Next
        .color1_255 = HexToUint32_unmapped("ffffff"),
        .color2_255 = HexToUint32_unmapped("9b2257"),
        .color3_255 = HexToUint32_unmapped("1e2329"),
        .color4_255 = HexToUint32_unmapped("ffffff"),
        .color5_255 = HexToUint32_unmapped("000000"),
        .color6_255 = HexToUint32_unmapped("ffffff"),
        .backgroundColor_255 = HexToUint32_unmapped("000000"),
        .thumbRadius = 20, // unscaled!

        .showClock = false,
        .clock24h = true,
        .showBatteryPercent = false,
        .showMenuAnimations = true,
        .showRecents = true,
        .showGameArt = true,
        .gameSwitcherScaling = GFX_SCALE_FULLSCREEN,

        .screenTimeoutSecs = 60,
        .suspendTimeoutSecs = 30,
    };

    *cfg = defaults;
}

void CFG_init(FontLoad_callback_t cb)
{
    CFG_defaults(&settings);
    settings.onFontChange = cb;
    bool fontLoaded = false;

    char settingsPath[MAX_PATH];
    sprintf(settingsPath, "%s/minuisettings.txt", SHARED_USERDATA_PATH);
    FILE *file = fopen(settingsPath, "r");
    if (file == NULL)
    {
        printf("[CFG] Unable to open settings file, loading defaults\n");
    }
    else
    {
        char line[256];
        while (fgets(line, sizeof(line), file))
        {
            int temp_value;
            uint32_t temp_color;
            if (sscanf(line, "font=%i", &temp_value) == 1)
            {
                CFG_setFontId(temp_value);
                fontLoaded = true;
                continue;
            }
            if (sscanf(line, "color1=%x", &temp_color) == 1)
            {
                char hexColor[7];
                snprintf(hexColor, sizeof(hexColor), "%06x", temp_color);
                CFG_setColor(1, HexToUint32_unmapped(hexColor));
                continue;
            }
            if (sscanf(line, "color2=%x", &temp_color) == 1)
            {
                CFG_setColor(2, temp_color);
                continue;
            }
            if (sscanf(line, "color3=%x", &temp_color) == 1)
            {
                CFG_setColor(3, temp_color);
                continue;
            }
            if (sscanf(line, "color4=%x", &temp_color) == 1)
            {
                CFG_setColor(4, temp_color);
                continue;
            }
            if (sscanf(line, "color5=%x", &temp_color) == 1)
            {
                CFG_setColor(5, temp_color);
                continue;
            }
            if (sscanf(line, "color6=%x", &temp_color) == 1)
            {
                CFG_setColor(6, temp_color);
                continue;
            }
            if (sscanf(line, "radius=%i", &temp_value) == 1)
            {
                CFG_setThumbnailRadius(temp_value);
                continue;
            }
            if (sscanf(line, "showclock=%i", &temp_value) == 1)
            {
                CFG_setShowClock((bool)temp_value);
                continue;
            }
            if (sscanf(line, "clock24h=%i", &temp_value) == 1)
            {
                CFG_setClock24H((bool)temp_value);
                continue;
            }
            if (sscanf(line, "batteryperc=%i", &temp_value) == 1)
            {
                CFG_setShowBatteryPercent((bool)temp_value);
                continue;
            }
            if (sscanf(line, "menuanim=%i", &temp_value) == 1)
            {
                CFG_setMenuAnimations((bool)temp_value);
                continue;
            }
            if (sscanf(line, "recents=%i", &temp_value) == 1)
            {
                CFG_setShowRecents((bool)temp_value);
                continue;
            }
            if (sscanf(line, "gameart=%i", &temp_value) == 1)
            {
                CFG_setShowGameArt((bool)temp_value);
                continue;
            }
            if (sscanf(line, "screentimeout=%i", &temp_value) == 1)
            {
                CFG_setScreenTimeoutSecs(temp_value);
                continue;
            }
            if (sscanf(line, "suspendTimeout=%i", &temp_value) == 1)
            {
                CFG_setSuspendTimeoutSecs(temp_value);
                continue;
            }
            if (sscanf(line, "switcherscale=%i", &temp_value) == 1)
            {
                CFG_setGameSwitcherScaling(temp_value);
                continue;
            }
        }
        fclose(file);
    }

    // load gfx related stuff until we drop the indirection
    CFG_setColor(1, CFG_getColor(1));
    CFG_setColor(2, CFG_getColor(2));
    CFG_setColor(3, CFG_getColor(3));
    CFG_setColor(4, CFG_getColor(4));
    // avoid reloading the font if not neccessary
    if (!fontLoaded)
        CFG_setFontId(CFG_getFontId());
}

int CFG_getFontId(void)
{
    return settings.font;
}

void CFG_setFontId(int id)
{
    settings.font = clamp(id, 0, 1);

    char *fontPath;
    if (settings.font == 1)
        fontPath = RES_PATH "/chillroundm.ttf";
    else
        fontPath = RES_PATH "/BPreplayBold-unhinted.otf";

    if(settings.onFontChange)
        settings.onFontChange(fontPath);
}

uint32_t CFG_getColor(int color_id)
{
    switch (color_id)
    {
    case 1:
        return settings.color1_255;
    case 2:
        return settings.color2_255;
    case 3:
        return settings.color3_255;
    case 4:
        return settings.color4_255;
    case 5:
        return settings.color5_255;
    case 6:
        return settings.color6_255;
    case 7:
        return settings.backgroundColor_255;
    default:
        return 0;
    }
}

void CFG_setColor(int color_id, uint32_t color)
{
    switch (color_id)
    {
    case 1:
        settings.color1_255 = color;
        THEME_COLOR1_255 = settings.color1_255;
        break;
    case 2:
        settings.color2_255 = color;
        THEME_COLOR2_255 = settings.color2_255;
        break;
    case 3:
        settings.color3_255 = color;
        THEME_COLOR3_255 = settings.color3_255;
        break;
    case 4:
        settings.color4_255 = color;
        THEME_COLOR4_255 = settings.color4_255;
        break;
    case 5:
        settings.color5_255 = color;
        THEME_COLOR5_255 = settings.color5_255;
        break;
    case 6:
        settings.color6_255 = color;
        THEME_COLOR6_255 = settings.color6_255;
        break;
    case 7:
        settings.backgroundColor_255 = color;
        break;
    default:
        break;
    }
}

uint32_t CFG_getScreenTimeoutSecs(void)
{
    return settings.screenTimeoutSecs;
}

void CFG_setScreenTimeoutSecs(uint32_t secs)
{
    settings.screenTimeoutSecs = secs;
}

uint32_t CFG_getSuspendTimeoutSecs(void)
{
    return settings.suspendTimeoutSecs;
}

void CFG_setSuspendTimeoutSecs(uint32_t secs)
{
    settings.suspendTimeoutSecs = secs;
}

bool CFG_getShowClock(void)
{
    return settings.showClock;
}

void CFG_setShowClock(bool show)
{
    settings.showClock = show;
}

bool CFG_getClock24H(void)
{
    return settings.clock24h;
}

void CFG_setClock24H(bool is24)
{
    settings.clock24h = is24;
}

bool CFG_getShowBatteryPercent(void)
{
    return settings.showBatteryPercent;
}

void CFG_setShowBatteryPercent(bool show)
{
    settings.showBatteryPercent = show;
}

bool CFG_getMenuAnimations(void)
{
    return settings.showMenuAnimations;
}

void CFG_setMenuAnimations(bool anims)
{
    settings.showMenuAnimations = anims;
}

int CFG_getThumbnailRadius(void)
{
    return settings.thumbRadius;
}

void CFG_setThumbnailRadius(int radius)
{
    settings.thumbRadius = clamp(radius, 0, 24);
}

bool CFG_getShowRecents(void)
{
    return settings.showRecents;
}

void CFG_setShowRecents(bool show)
{
    settings.showRecents = show;
}

bool CFG_getShowGameArt(void)
{
    return settings.showGameArt;
}

void CFG_setShowGameArt(bool show)
{
    settings.showGameArt = show;
}

int CFG_getGameSwitcherScaling(void)
{
    return settings.gameSwitcherScaling;
}

void CFG_setGameSwitcherScaling(int enumValue)
{
    settings.gameSwitcherScaling = clamp(enumValue, 0, GFX_SCALE_NUM_OPTIONS);
}

void CFG_get(const char *key, char *value)
{
    if (strcmp(key, "font") == 0)
    {
        sprintf(value, "%i", CFG_getFontId());
    }
    else if (strcmp(key, "color1") == 0)
    {
        sprintf(value, "0x%06X", CFG_getColor(1));
    }
    else if (strcmp(key, "color2") == 0)
    {
        sprintf(value, "0x%06X", CFG_getColor(2));
    }
    else if (strcmp(key, "color3") == 0)
    {
        sprintf(value, "0x%06X", CFG_getColor(3));
    }
    else if (strcmp(key, "color4") == 0)
    {
        sprintf(value, "0x%06X", CFG_getColor(4));
    }
    else if (strcmp(key, "color5") == 0)
    {
        sprintf(value, "0x%06X", CFG_getColor(5));
    }
    else if (strcmp(key, "color6") == 0)
    {
        sprintf(value, "0x%06X", CFG_getColor(6));
    }
    else if (strcmp(key, "bgcolor") == 0)
    {
        sprintf(value, "0x%06X", CFG_getColor(7));
    }
    else if (strcmp(key, "radius") == 0)
    {
        sprintf(value, "%i", CFG_getThumbnailRadius());
    }
    else if (strcmp(key, "showclock") == 0)
    {
        sprintf(value, "%i", CFG_getShowClock());
    }
    else if (strcmp(key, "clock24h") == 0)
    {
        sprintf(value, "%i", CFG_getClock24H());
    }
    else if (strcmp(key, "batteryperc") == 0)
    {
        sprintf(value, "%i", CFG_getShowBatteryPercent());
    }
    else if (strcmp(key, "menuanim") == 0)
    {
        sprintf(value, "%i", CFG_getMenuAnimations());
    }
    else if (strcmp(key, "recents") == 0)
    {
        sprintf(value, "%i", CFG_getShowRecents());
    }
    else if (strcmp(key, "gameart") == 0)
    {
        sprintf(value, "%i", CFG_getShowGameArt());
    }
    else if (strcmp(key, "screentimeout") == 0)
    {
        sprintf(value, "%i", CFG_getScreenTimeoutSecs());
    }
    else if (strcmp(key, "suspendTimeout") == 0)
    {
        sprintf(value, "%i", CFG_getSuspendTimeoutSecs());
    }
    else if (strcmp(key, "switcherscale") == 0)
    {
        sprintf(value, "%i", CFG_getGameSwitcherScaling());
    }

    // meta, not a real setting
    else if (strcmp(key, "fontpath") == 0)
    {
        if (CFG_getFontId() == 1)
            sprintf(value, "\"%s\"", RES_PATH "/chillroundm.ttf");
        else
            sprintf(value, "\"%s\"", RES_PATH "/BPreplayBold-unhinted.otf");
    }

    else {
        sprintf(value, "");
    }
}

void CFG_sync(void)
{
    // write to file
    char settingsPath[MAX_PATH];
    sprintf(settingsPath, "%s/minuisettings.txt", getenv("SHARED_USERDATA_PATH"));
    FILE *file = fopen(settingsPath, "w");
    if (file == NULL)
    {
        printf("[CFG] Unable to open settings file, cant write\n");
        return;
    }

    fprintf(file, "font=%i\n", settings.font);
    fprintf(file, "color1=0x%06X\n", settings.color1_255);
    fprintf(file, "color2=0x%06X\n", settings.color2_255);
    fprintf(file, "color3=0x%06X\n", settings.color3_255);
    fprintf(file, "color4=0x%06X\n", settings.color4_255);
    fprintf(file, "color5=0x%06X\n", settings.color5_255);
    fprintf(file, "color6=0x%06X\n", settings.color6_255);
    fprintf(file, "bgcolor=0x%06X\n", settings.backgroundColor_255);
    fprintf(file, "radius=%i\n", settings.thumbRadius);
    fprintf(file, "showclock=%i\n", settings.showClock);
    fprintf(file, "clock24h=%i\n", settings.clock24h);
    fprintf(file, "batteryperc=%i\n", settings.showBatteryPercent);
    fprintf(file, "menuanim=%i\n", settings.showMenuAnimations);
    fprintf(file, "recents=%i\n", settings.showRecents);
    fprintf(file, "gameart=%i\n", settings.showGameArt);
    fprintf(file, "screentimeout=%i\n", settings.screenTimeoutSecs);
    fprintf(file, "suspendTimeout=%i\n", settings.suspendTimeoutSecs);
    fprintf(file, "switcherscale=%i\n", settings.gameSwitcherScaling);

    fclose(file);
}

void CFG_print(void)
{
    printf("{\n");
    printf("\t\"font\": %i\n", settings.font);
    printf("\t\"color1\": \"0x%06X\"\n", settings.color1_255);
    printf("\t\"color2\": \"0x%06X\"\n", settings.color2_255);
    printf("\t\"color3\": \"0x%06X\"\n", settings.color3_255);
    printf("\t\"color4\": \"0x%06X\"\n", settings.color4_255);
    printf("\t\"color5\": \"0x%06X\"\n", settings.color5_255);
    printf("\t\"color6\": \"0x%06X\"\n", settings.color6_255);
    printf("\t\"bgcolor\": \"0x%06X\"\n", settings.backgroundColor_255);
    printf("\t\"radius\": %i\n", settings.thumbRadius);
    printf("\t\"showclock\": %i\n", settings.showClock);
    printf("\t\"clock24h\": %i\n", settings.clock24h);
    printf("\t\"batteryperc\": %i\n", settings.showBatteryPercent);
    printf("\t\"menuanim\": %i\n", settings.showMenuAnimations);
    printf("\t\"recents\": %i\n", settings.showRecents);
    printf("\t\"gameart\": %i\n", settings.showGameArt);
    printf("\t\"screentimeout\": %i\n", settings.screenTimeoutSecs);
    printf("\t\"suspendTimeout\": %i\n", settings.suspendTimeoutSecs);
    printf("\t\"switcherscale\": %i\n", settings.gameSwitcherScaling);

    // meta, not a real setting
    if (settings.font == 1)
        printf("\t\"fontpath\": \"%s\"\n", RES_PATH "/chillroundm.ttf");
    else
        printf("\t\"fontpath\": \"%s\"\n", RES_PATH "/BPreplayBold-unhinted.otf");

    printf("}\n");
}

void CFG_quit(void)
{
    CFG_sync();
}