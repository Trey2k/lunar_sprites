local m_window = nil

local m_sprite = nil
local m_camera = nil

local m_should_stop = false

local function deg_to_rad(degrees)
    return degrees * (CORE.PI / 180.0)
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
    CORE.event_manager:add_handler(input_handler)
    RENDERER:set_clear_color(0.0, 0.0, 0.0, 1.0)

    m_sprite = RENDERER:new_sprite("../../../bin/moon.png", vec2(0, 0), vec2(0.25, 0.25), 0.0)
    viewport_size = RENDERER:get_viewport_size()
    m_camera = RENDERER:new_camera(deg_to_rad(100), viewport_size.y / viewport_size.x, 0.1, 100.0)
    m_camera:set_active()
end

local timer = 0.0

function update(delta)
    m_sprite.rotation = m_sprite.rotation + (0.1 * delta)
    if m_sprite.rotation >= CORE.PI * 2.0 then
        m_sprite.rotation = 0.0
    end

    if CORE.input_manager:is_key_just_pressed(CORE.KEY_W) then
        m_camera:move(vec3(0.0, 0.1, 0.0))
    end

    if CORE.input_manager:is_key_just_pressed(CORE.KEY_S) then
        m_camera:move(vec3(0.0, -0.1, 0.0))
    end

    if CORE.input_manager:is_key_just_pressed(CORE.KEY_A) then
        m_camera:move(vec3(-0.1, 0.0, 0.0))
    end

    if CORE.input_manager:is_key_just_pressed(CORE.KEY_D) then
        m_camera:move(vec3(0.1, 0.0, 0.0))
    end

    timer = timer + delta
    check_input()

    m_sprite:draw()

    if timer >= 1.0 then
        print("FPS: "..tostring(1.0 / delta))
        timer = 0.0
    end
end

function should_stop()
    return m_should_stop
end

function check_input()
    if CORE.input_manager:is_key_just_pressed(CORE.KEY_ESCAPE) or 
         CORE.input_manager:is_key_just_pressed(CORE.KEY_Q) then
        m_should_stop = true
    end
end

function input_handler(e)
    if e.type == CORE.EVENT_WINDOW_CLOSE then
        m_should_stop = true
    elseif e.type == CORE.EVENT_KEY then
        if e.key.type == CORE.EVENT_KEY_PRESSED then
            if not e.key.repeated then
                print("Key pressed: "..CORE.keycode_to_string(e.key.keycode))
            else
                print("Key repeat: "..CORE.keycode_to_string(e.key.keycode))
            end
        elseif e.key.type == CORE.EVENT_KEY_RELEASED then
            print("Key released: "..CORE.keycode_to_string(e.key.keycode))
        end
    
    elseif e.type == CORE.EVENT_MOUSE then
        if e.mouse.type == CORE.EVENT_MOUSE_PRESSED then
            print("Mouse pressed: "..CORE.mouse_button_to_string(e.mouse.button))
        elseif e.mouse.type == CORE.EVENT_MOUSE_RELEASED then
            print("Mouse released: "..CORE.mouse_button_to_string(e.mouse.button))
        elseif e.mouse.type == CORE.EVENT_MOUSE_MOVED then
            print("Mouse moved: "..tostring(e.mouse.position.x)..", "..tostring(e.mouse.position.y))
        elseif e.mouse.type == CORE.EVENT_MOUSE_ENTERED then
            print("Mouse entered: "..tostring(e.mouse.position.x)..", "..tostring(e.mouse.position.y))
        elseif e.mouse.type == CORE.EVENT_MOUSE_LEFT then
            print("Mouse left: "..tostring(e.mouse.position.x)..", "..tostring(e.mouse.position.y))
        end
    end
end


function deinit()
    m_sprite = nil
    m_window = nil
end