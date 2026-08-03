
/*
 * PluginClient.cc
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (C) 2026 brummer <brummer@web.de>
 */

#include "NeuralRack.cc"
#include "PluginAPI.h"

#if defined (VST3IPLUG)
#include "travesty/base.h"
#elif defined (CLAPIPLUG)
#include "clap/plugin-features.h"
#elif defined (VST2IPLUG)
#include "vestige.h"
#endif

#include <cstring>


class NeuralRackClient : public IPluginClient {
public:
    void initEngine(uint32_t sampleRate, int32_t priority, int32_t policy) override {
        neuralRack.initEngine(sampleRate, priority, policy);
    }

    void selectVariant(int /*variantIndex*/) override {}

    uint32_t getLatencySamples() const override {
        uint32_t latency = 0;
        const_cast<NeuralRack&>(neuralRack).getLatency(&latency);
        return latency;
    }

    void process(uint32_t nframes, float* const* inputs, uint32_t numInputs,
                                    float* const* outputs, uint32_t numOutputs) override {

        float* const left = numOutputs > 0 ? outputs[0] : nullptr;
        float* const right = numOutputs > 1 ? outputs[1] : left;
        const float* const in = numInputs > 0 ? inputs[0] : nullptr;

        if (in != nullptr) {
            if (left != nullptr && left != in) std::memcpy(left, in, nframes * sizeof(float));
            if (right != nullptr && right != in) std::memcpy(right, in, nframes * sizeof(float));
        }

        neuralRack.process(nframes, left, right);
    }

    Params& params() override { return neuralRack.param; }

    void readState(const std::string& state) override { neuralRack.readState(state); }
    void saveState(std::string* state) override { neuralRack.saveState(state); }

    void startGui() override { neuralRack.startGui(); }

    void startGui(WindowHandle parent) override {
        neuralRack.startGui(reinterpret_cast<Window>(parent));
    }

    void showGui() override { neuralRack.showGui(); }
    void hideGui() override { neuralRack.hideGui(); }
    void quitGui() override { neuralRack.quitGui(); }

    void setParent(WindowHandle parent) override {
        neuralRack.setParent(reinterpret_cast<Window>(parent));
    }

    void getGuiSize(int& width, int& height) const override {
        if (neuralRack.TopWin != nullptr) {
            width = static_cast<int>(neuralRack.TopWin->width);
            height = static_cast<int>(neuralRack.TopWin->height);
        } else {
            width = 620;
            height = 580;
        }
    }

    bool resizeGui(int width, int height) override {
        if (neuralRack.TopWin == nullptr) return false;
        os_resize_window(neuralRack.getMain()->dpy, neuralRack.TopWin, width, height);
        return true;
    }

    bool setGuiScale(double scale) override {
        neuralRack.getMain()->hdpi = scale;
        return true;
    }

private:
    NeuralRack neuralRack;
};

// PluginDescriptor
static PluginDescriptor buildDescriptor() {
    PluginDescriptor d{};
    d.vendor           = "brummer10";
    d.url              = "https://github.com/brummer10/NeuralRack";
    d.email            = "mailto:brummer-@web.de";

#if defined (VST3IPLUG)
    d.vst3Category      = "Audio Module Class";
    d.vst3SubCategories = "Fx|Distortion";
    d.vst3SdkVersion    = "VST 3.7.9";
#elif defined (CLAPIPLUG)
    static const char *features[] = { CLAP_PLUGIN_FEATURE_AUDIO_EFFECT , CLAP_PLUGIN_FEATURE_DISTORTION, NULL};
    d.clapFeature      = features;
#endif
    d.version          = "0.4.1";

    // NeuralRack is a mono-in/stereo-out amp+cab simulator.
    d.numInputChannels  = 1;
    d.numOutputChannels = 2;

    PluginVariantInfo main{};
    main.id            = "com.brummer10.NeuralRack";
    main.name           = "NeuralRack";
    main.description    = "Neural amp modeler with cabinet IR and EQ";

#if defined (VST3IPLUG)
    {
        v3_tuid uid = V3_ID(0x88736da6, 0xb6ec4274, 0x1e72494e, 0xc06a5d85);
        std::memcpy(main.vst3Uid, uid, sizeof(main.vst3Uid));
    }
#endif

#if defined (VST2IPLUG)
    main.vst2UniqueId = CCONST('N', 'r', 'b', 'r');
#endif
    d.variants = { main };
    return d;
}

const PluginDescriptor& getPluginDescriptor() {
    static const PluginDescriptor d = buildDescriptor();
    return d;
}

IPluginClient* createPluginInstance(int /*variantIndex*/) {
    return new NeuralRackClient();
}
