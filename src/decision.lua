-- Adaptive Weapon Tactics - runtime-independent decision engine
-- Lua 5.1-compatible style. This module does not call Kenshi/RE_Kenshi APIs.
--
-- Adapter supplies normalized snapshots:
-- snapshot = {
--   enabled = true,
--   manual_mode = "auto" | "primary" | "secondary",
--   current_slot = "primary" | "secondary",
--   seconds_since_switch = number,
--   environment = { indoor = boolean, cramped = boolean },
--   self = { injured_arms = number, conscious = boolean },
--   enemies = {
--     { distance = number, armor = number [0..1], blunt_resistance = number [0..1],
--       cut_resistance = number [0..1], is_robot = boolean, threat = number [0..1],
--       x = number, z = number }
--   },
--   weapons = {
--     primary = { available = boolean, reach = number, cut = number, blunt = number,
--       armor_pen = number, indoor_penalty = number, skill = number [0..1],
--       speed = number, cleave = number [0..1], robot_bonus = number },
--     secondary = { same fields }
--   }
-- }
--
-- decide(snapshot, config) returns { slot=..., reason=..., scores=... }.
-- A nil slot means keep the current weapon.

local M = {}

local function clamp(x, lo, hi)
    if x < lo then return lo end
    if x > hi then return hi end
    return x
end

local function average_enemy_armor(enemies)
    if not enemies or #enemies == 0 then return 0 end
    local sum = 0
    for _, enemy in ipairs(enemies) do
        sum = sum + clamp(enemy.armor or 0, 0, 1)
    end
    return sum / #enemies
end

local function crowd_factor(enemies, radius)
    if not enemies or #enemies == 0 then return 0 end
    local nearby = 0
    for _, enemy in ipairs(enemies) do
        if (enemy.distance or math.huge) <= radius then nearby = nearby + 1 end
    end
    return clamp((nearby - 1) / 4, 0, 1)
end

local function distance_fit(weapon, enemies)
    if not enemies or #enemies == 0 then return 0.5 end
    local nearest = math.huge
    for _, enemy in ipairs(enemies) do
        nearest = math.min(nearest, enemy.distance or math.huge)
    end
    local reach = math.max(weapon.reach or 1, 0.1)
    -- Reward weapons whose reach is proportionate to the nearest target distance.
    return clamp(1 - math.abs(reach - nearest) / math.max(reach, nearest, 1), 0, 1)
end

local function score_weapon(weapon, snapshot, cfg)
    if not weapon or not weapon.available then return -math.huge end
    local enemies = snapshot.enemies or {}
    if #enemies == 0 then return 0 end

    local armor = average_enemy_armor(enemies)
    local crowd = crowd_factor(enemies, cfg.crowd_radius)
    local cut = math.max(weapon.cut or 0, 0)
    local blunt = math.max(weapon.blunt or 0, 0)
    local pen = clamp(weapon.armor_pen or 0, -1, 1)
    local skill = clamp(weapon.skill or 0, 0, 1)
    local speed = math.max(weapon.speed or 1, 0)
    local cleave = clamp(weapon.cleave or 0, 0, 1)
    local robot_bonus = weapon.robot_bonus or 0

    local robot_ratio, threat_sum = 0, 0
    for _, enemy in ipairs(enemies) do
        if enemy.is_robot then robot_ratio = robot_ratio + 1 end
        threat_sum = threat_sum + clamp(enemy.threat or 0.5, 0, 1)
    end
    robot_ratio = robot_ratio / #enemies
    local cut_resistance, blunt_resistance = 0, 0
    for _, enemy in ipairs(enemies) do
        cut_resistance = cut_resistance + clamp(enemy.cut_resistance or 0.5, 0, 1)
        blunt_resistance = blunt_resistance + clamp(enemy.blunt_resistance or 0.5, 0, 1)
    end
    cut_resistance = cut_resistance / #enemies
    blunt_resistance = blunt_resistance / #enemies

    local damage_fit = cut * (1 - cut_resistance) + blunt * (1 - blunt_resistance)
    local armor_fit = (blunt * (1 - armor * 0.65) + cut * (1 - armor) + pen * cfg.penetration_scale)
    local crowd_fit = cleave * crowd + speed * (1 - crowd) * cfg.speed_scale
    local environment_fit = 1
    if snapshot.environment and snapshot.environment.indoor then
        environment_fit = environment_fit - (weapon.indoor_penalty or 0)
        if snapshot.environment.cramped then
            environment_fit = environment_fit - (weapon.cramped_penalty or weapon.indoor_penalty or 0)
        end
    end
    environment_fit = clamp(environment_fit, 0, 1.25)
    local distance = distance_fit(weapon, enemies)
    local robot_fit = robot_ratio * robot_bonus
    local threat_fit = threat_sum / #enemies
    local injury_penalty = 0
    if snapshot.self and (snapshot.self.injured_arms or 0) > 0 then
        injury_penalty = (snapshot.self.injured_arms or 0) * cfg.injured_arm_penalty
    end

    return cfg.weights.damage * damage_fit
        + cfg.weights.armor * armor_fit
        + cfg.weights.crowd * crowd_fit
        + cfg.weights.environment * environment_fit
        + cfg.weights.distance * distance
        + cfg.weights.skill * skill
        + cfg.weights.robot * robot_fit
        + cfg.weights.threat * threat_fit
        - injury_penalty
end

function M.score(snapshot, config)
    local cfg = config or {}
    local weapons = snapshot.weapons or {}
    return {
        primary = score_weapon(weapons.primary, snapshot, cfg),
        secondary = score_weapon(weapons.secondary, snapshot, cfg)
    }
end

function M.decide(snapshot, config)
    local cfg = config or {}
    if not snapshot or snapshot.enabled == false then
        return { slot = nil, reason = "disabled" }
    end
    if snapshot.self and snapshot.self.conscious == false then
        return { slot = nil, reason = "character_unconscious" }
    end

    local mode = snapshot.manual_mode or "auto"
    if mode == "primary" or mode == "secondary" then
        local weapon = snapshot.weapons and snapshot.weapons[mode]
        if weapon and weapon.available then
            return { slot = mode, reason = "manual_lock" }
        end
        return { slot = nil, reason = "manual_weapon_unavailable" }
    end

    if not snapshot.enemies or #snapshot.enemies == 0 then
        return { slot = nil, reason = "no_enemies" }
    end

    local scores = M.score(snapshot, cfg)
    local current = snapshot.current_slot
    local other = current == "primary" and "secondary" or "primary"
    if not snapshot.weapons or not snapshot.weapons[current] or not snapshot.weapons[current].available then
        if scores.primary > -math.huge then return { slot = "primary", reason = "current_unavailable", scores = scores } end
        if scores.secondary > -math.huge then return { slot = "secondary", reason = "current_unavailable", scores = scores } end
        return { slot = nil, reason = "no_available_weapon", scores = scores }
    end

    local current_score = scores[current]
    local other_score = scores[other]
    local delta = (cfg.switch_margin or 0.15) * math.max(math.abs(current_score), 1)
    if other_score > current_score + delta then
        if (snapshot.seconds_since_switch or math.huge) < (cfg.switch_cooldown_seconds or 3) then
            return { slot = nil, reason = "cooldown", scores = scores }
        end
        return { slot = other, reason = "better_score", scores = scores }
    end

    return { slot = nil, reason = "keep_current", scores = scores }
end

return M
