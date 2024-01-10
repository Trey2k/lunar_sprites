local m_core = nil
local m_renderer = nil
local m_window = nil

function init(core, renderer)
    m_core = core
    m_renderer = renderer

    m_window = m_renderer:new_window({
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
    m_renderer:set_clear_color(0.0, 0.0, 0.0, 1.0)
end

local time = 0.0

function update(delta)
    time = time + delta
    if time >= 1.0 then
        time = 0.0
        print("FPS: "..tostring(1.0 / delta))
    end

    m_renderer:set_clear_color(time, 0.0, 0.0, 1.0)
end

function should_stop()
    return false
end

function deinit()
    print("deinit")
end