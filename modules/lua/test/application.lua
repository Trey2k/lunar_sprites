local m_window = nil

local m_sprite = nil
local m_camera = nil

local function deg_to_rad(degrees)
    return degrees * (LS.PI / 180.0)
end

function init()
    m_window = RENDERER:new_window({
        position = vec2i(0, 0),
        size = vec2i(800, 600),
        title = "Lunar Sprites Test Application",
        fullscreen = false,
        hidden = false,
        root_window = true,
    })

    return m_window
end

function start()
    print("Key:", LS.KEY_A)
    RENDERER:set_clear_color(0.0, 0.0, 0.0, 1.0)

    m_sprite = RENDERER:new_sprite("../../../bin/moon.png", vec2(0, 0), vec2(0.25, 0.25), 0.0)
    viewport_size = RENDERER:get_viewport_size()
    m_camera = RENDERER:new_camera(deg_to_rad(100), viewport_size.y / viewport_size.x, 0.1, 100.0)
    m_camera:set_active()
end

local time = 0.0

function update(delta)
    m_sprite.rotation = m_sprite.rotation + (0.1 * delta)
    if m_sprite.rotation >= LS.PI * 2.0 then
        m_sprite.rotation = 0.0
    end

    m_sprite:draw()

    time = time + delta
    if time >= 1.0 then
        time = 0.0
        print("FPS: "..tostring(1.0 / delta))
    end
end

function should_stop()
    return false
end

function deinit()
    m_sprite = nil
    m_window = nil
end