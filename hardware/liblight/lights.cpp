/*
 * Copyright (C) 2020 The Android Open Source Project
 * Copyright (C) 2020 Pavel Dubrova <pashadubrova@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <log/log.h>
#include <fstream>
#include <string>
#include <unistd.h>

#include <hardware/lights.h>

static pthread_once_t g_init = PTHREAD_ONCE_INIT;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

static const std::string LCD_BRIGHTNESS_FILE
        = "/sys/class/backlight/1a98000.dsi.0/brightness";

static const std:: string LEDS_BRIGHTNESS_FILE
        = "/sys/class/leds/%s/brightness";

enum rgb_led {
    LED_RED = 0,
    LED_GREEN,
    LED_BLUE,
    LED_WHITE
};

static const std::string led_names[] = {
    "red",
    "green",
    "blue",
    "white",
};

void init_globals(void)
{
    /* Init the mutex */
    pthread_mutex_init(&g_lock, NULL);
}

static bool file_exists(const std::string &path)
{
    std::ofstream stream(path, std::ios_base::out);

    if (!stream.is_open()) {
        ALOGE("%s: failed to open %s, errno = %d\n",
                __func__, path.c_str(), errno);
        return false;
    }

    stream.close();

    return true;
}

static int write_int(const std::string &path, int value)
{
    if (file_exists(path)) {
        std::ofstream stream(path, std::ios_base::out);
        stream << value << std::endl;
        stream.close();
    }

    return 0;
}

/*
static int write_int(const std::string &path, const std::string &value)
{
    if (file_exists(path)) {
        std::ofstream stream(path, std::ios_base::out);
        stream << value << std::endl;
        stream.close();
    }

    return 0;
}
*/

static int rgb_to_brightness(struct light_state_t const* state)
{
    int color = state->color & 0x00ffffff;
    return ((77*((color>>16)&0x00ff))
            + (150*((color>>8)&0x00ff)) + (29*(color&0x00ff))) >> 8;
}

static int set_light_backlight(struct light_device_t* dev,
        struct light_state_t const* state)
{
    int brightness = rgb_to_brightness(state);
    int rc = 0;

    if(!dev)
        return -1;

    pthread_mutex_lock(&g_lock);
    rc = write_int(LCD_BRIGHTNESS_FILE, brightness);
    pthread_mutex_unlock(&g_lock);

    return rc;
}

static void set_rgb_led_brightness(enum rgb_led led, int brightness)
{
    std::string file =
            "/sys/class/leds/" + led_names[led] + "/brightness";

    write_int(file, brightness);
}

static int set_light_battery(struct light_device_t* dev,
        struct light_state_t const* state)
{
    unsigned int red, green, blue, colorRGB;
    int rc = 0;

    if (!dev)
        return -1;

    colorRGB = state->color;
    red = (colorRGB >> 16) & 0xFF;
    green = (colorRGB >> 8) & 0xFF;
    blue = colorRGB & 0xFF;

    set_rgb_led_brightness(LED_RED, red);
    set_rgb_led_brightness(LED_GREEN, green);
    set_rgb_led_brightness(LED_BLUE, blue);

    ALOGD("set_speaker_light_locked mode=%d, colorRGB=%08X, rc=%d\n",
            state->flashMode, colorRGB, rc);

    return rc;
}

static int close_lights(struct light_device_t *dev)
{
    if (dev)
        free(dev);

    return 0;
}

static int open_lights(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    int (*set_light)(struct light_device_t* dev,
            struct light_state_t const* state);

    if (strcmp(LIGHT_ID_BACKLIGHT, name) == 0)
        set_light = set_light_backlight;
    else if (strcmp(LIGHT_ID_BATTERY, name) == 0)
        set_light = set_light_battery;
    else
        return -EINVAL;

    pthread_once(&g_init, init_globals);

    struct light_device_t *dev =
            (light_device_t *)malloc(sizeof(struct light_device_t));

    if(!dev)
        return -ENOMEM;

    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = LIGHTS_DEVICE_API_VERSION_2_0;
    dev->common.module = (struct hw_module_t*)module;
    dev->common.close = (int (*)(struct hw_device_t*))close_lights;
    dev->set_light = set_light;

    *device = (struct hw_device_t*)dev;

    return 0;
}

static struct hw_module_methods_t lights_module_methods = {
    .open =  open_lights,
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = LIGHTS_HARDWARE_MODULE_ID,
    .name = "lights Module",
    .author = "Google, Inc.",
    .methods = &lights_module_methods,
};
