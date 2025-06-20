# Engine: Real-Time Sensory Frontend for Physically Grounded Worlds

> _“Where simulated truth becomes felt reality.”_

This **Engine** is the interactive, perceptual layer that transforms offline, physically accurate data — eventually generated by the [`PhysicsSolverSDK`](https://github.com/swithmario/PhysicsSolverSDK) — into a rich, immersive experience. It is the bridge between computation and sensation, simulation and perception.

---

## 🎯 Dual Purpose, Unified Vision

| | `PhysicsSolverSDK` | `Engine` |
|--|--------------------|----------|
| **Role** | The Scientist – Simulates physical truth | The Performer – Delivers experiential reality |
| **Output** | Acoustic/light fields, stress maps, spectral data | Real-time visuals, spatialized sound, user interaction |
| **Process** | Offline, computationally intense | Real-time, interactive, responsive |
| **Focus** | Ground-truth simulation | Perceptual fidelity and interactivity |

---

## 🧠 Philosophy: Why the Engine Matters

While the `PhysicsSolverSDK` crunches numbers and bakes precision, **this Engine brings those simulations to life**.

It is responsible for:
- **Presentation**: Converting raw data into visuals, audio, and effects that a user can perceive.
- **Interaction**: Allowing users to navigate and interact with a space pre-defined by baked physics.
- **Real-Time Enhancement**: Layering dynamic responses and procedural feedback on top of static simulation output.
- **Perceptual Realism**: Creating a world that feels *viscerally correct*, even if built from precomputed data.

> Without the Engine, even the most detailed simulation is inert. This is where it becomes *real*.

---

## 🔊 Current Focus: L-Hallway Audio MVP

We're starting with a focused goal: **acoustic perception**.

The L-Hallway MVP tests the pipeline by:
- Embedding pre-computed acoustic data directly in C++.
- Using a listener and sound source to simulate movement through a space.
- Dynamically modulating audio (attenuation, basic reverb) based on listener proximity to audio probes.
- Visualizing debug data using Dear ImGui.

> ✅ A critical first step toward using baked physics to drive real-time sensory feedback.

---

## 🚧 Roadmap (MVP Phase)

- ✅ Finalize CMake setup (no hardcoded SDL2/SDL2_mixer paths)
- ✅ Add `AcousticDataTypes.h` and embedded probe source
- ✅ Implement `AudioProbeManager`
- ✅ Implement `AudioSourceComponent` and `AudioListenerComponent`
- ✅ Integrate `AudioSystem` with main application loop
- ⏳ Build ImGui debug panel (listener, source, probe info)
- ⏳ Refine basic reverb or delay modeling
- ⏳ Support (future) live reload of baked acoustic files (USD support)

---

## 💻 Getting Started

### ✅ Requirements (macOS Ventura+)

```bash
brew install sdl2 sdl2_mixer glm
```

- CMake ≥ 3.16
- Xcode Command Line Tools
- C++17 compiler

### 🛠️ Build Instructions

```bash
git clone https://github.com/swithmario/Engine.git
cd Engine
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

Then run:
```bash
./MyEngineApp.app/Contents/MacOS/MyEngineApp
```

> You may rename the app target later for consistency with this repo name.

---

## 🤝 For Contributors

You're welcome to help shape this project! Here are a few starting points:

- Improve cross-platform CMake support
- Add ImGui panels for visualizing audio probe influence
- Add unit tests for audio components
- Refactor and document core architecture

A full `CONTRIBUTING.md` is planned soon.

---

## 🪪 License

This project is yet to be licensed, but likely under the [MIT License](./LICENSE). Feel free to use, extend, or remix responsibly.

---

This Engine is **not just an audio engine** — it is the perceptual interpreter of physics. Whether you're hearing sound bounce around corners or seeing light scatter spectrally, this system aims to **faithfully deliver the experience of real physics, in real-time, for real people**.
