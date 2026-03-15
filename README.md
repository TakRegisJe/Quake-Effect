# Quake Effect - Mod Changes

This document describes all gameplay changes made to the base Quake 4 game as part of this mod.

---

## Weapons

### Blaster
- **Damage increased**: from 8 to 20 per shot.
- **Fire rate reduced**: from 0.15s to 0.3s between shots, making it a slower, more deliberate sidearm.
- **Ammo system added**: the blaster now consumes ammo instead of having infinite shots.
  - Clip size: 12 rounds.
  - Starting ammo: 30 rounds.
  - Maximum ammo capacity: 60 rounds.
- **Ammo HUD enabled**: a blue energy-themed ammo counter is now displayed when the blaster is equipped, with color-coded warnings for low ammo and empty states.

---

### Machinegun
- **Ammo system removed**: the machinegun no longer uses ammo pickups or clips.
- **Overheat system added**: firing builds up heat. If heat reaches maximum, the gun locks out until it fully cools down.
  - Each shot adds 10 heat (out of 100 max).
  - At 10 shots of sustained fire the gun overheats and becomes unusable.
  - Heat drains at 25 units per second whenever not at maximum, including during firing pauses — rewarding burst fire over sustained fire.
  - Once fully cooled, the gun is ready to fire again automatically.

---

### Shotgun
- **Damage doubled**: each pellet now deals 28 damage (was 14). With 10 pellets per shot, maximum damage per shot is 280.
- **Spread tripled**: pellet spread increased from 7 to 21, making the shotgun a close-range weapon with a much wider blast cone.

---

### Railgun
- **Damage doubled**: from 125 to 250 per shot.
- **Clip reduced to 1 round**: the railgun now holds a single shot before reloading, reinforcing its identity as a high-risk, high-reward precision weapon.

---

### Grenade Launcher
- **Grenades now stick to enemies**: on contact with an actor, the grenade attaches to them and travels with them until the fuse expires.
- **Fuse time increased**: from 2.5 seconds to 4 seconds, giving stuck grenades time to travel with an enemy before detonating.

---

## Player Systems

### Stats Upgrade System
- Players can open a stats upgrade menu during gameplay to spend points on character upgrades.
- Upgradeable stats include:
  - **Armor** — reduces incoming damage.
  - **Speed** — increases player movement speed.
  - **Ability** — reduces ability recharge time.
  - **Awareness** — controls the strength of the time-slow effect.

---

### Companion System
- Players can spawn squad companions via keybinds during gameplay.
- Available companions: **Kasumi**, **Garrus**, **Tali**, **Mordin**, and **Liara**.
- Companions fight alongside the player and are tracked as active squad members.
- When a companion dies, they are removed from the active squad automatically.
- Companion and enemy health is displayed above their heads as a floating number.

---

### Abilities
Players have access to the following active abilities:

- **Cloak** — toggles stealth, making the player undetectable by enemies.
- **Dominate** — converts all visible enemies in range to fight on the player's side.
- **Incinerate** — launches a guided incendiary projectile that homes in on the nearest enemy.
- **Heal Companions** — restores 50 health to each active companion in the squad.
