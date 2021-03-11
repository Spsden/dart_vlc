#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>
#include "include/flutter_vlc/flutter_vlc_plugin.h"
#include "include/flutter_types.hpp"

#include "main.cpp"


std::unique_ptr<flutter::MethodChannel<flutter::EncodableValue>> channel;


namespace {

    class FlutterVlcPlugin : public flutter::Plugin {
    public:
        static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

        FlutterVlcPlugin();

        virtual ~FlutterVlcPlugin();

    private:
        void HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &method_call, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
    };

    void FlutterVlcPlugin::RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar) {
        channel = std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(registrar->messenger(), "flutter_vlc", &flutter::StandardMethodCodec::GetInstance());
        auto plugin = std::make_unique<FlutterVlcPlugin>();
        channel->SetMethodCallHandler(
            [plugin_pointer = plugin.get()](const auto &call, auto result) {
                plugin_pointer->HandleMethodCall(call, std::move(result));
            }
        );
        registrar->AddPlugin(std::move(plugin));
    }

    FlutterVlcPlugin::FlutterVlcPlugin() {}

    FlutterVlcPlugin::~FlutterVlcPlugin() {}

    void FlutterVlcPlugin::HandleMethodCall(const flutter::MethodCall<flutter::EncodableValue> &methodCall, std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
        Method* method = new Method(&methodCall, std::move(result));
        if (method->name == "open") {
            /*
             * Opens an [AudioSource] i.e [Audio] or [Playlist] into the [AudioPlayer].
             * 
             * Argument for loading an [Audio]:
             * 
             * {
             *      'id': 0,
             *      'type': 'audio',
             *      'audio': {
             *          'type': 'file',
             *          'resource': 'C:/alexmercerind/music.MP3'
             *      }
             * }
             * 
             * Argument for loading a [Playlist]:
             * 
             * {
             *      'id': 1,
             *      'type': 'playlist',
             *      'start': 0,
             *      'playlist': [
             *          {
             *              'type': 'network',
             *              'resource': 'https://alexmercerind.com/music.MP3'
             *          },
             *          {
             *              'type': 'file',
             *              'resource': 'C:/alexmercerind/music.MP3'
             *          }
             *      ]
             * }
             */
            int id = method->getArgument<int>("id");
            std::string type = method->getArgument<std::string>("type");
            if (type == "audio") {
                std::map<std::string,std::string> audioMap = method->getArgument<std::map<std::string,std::string>>("audio");
                Audio* audio;
                if (audioMap["type"] == "file")
                    audio = Audio::file(audioMap["resource"]);
                if (audioMap["type"] == "network")
                    audio = Audio::network(audioMap["resource"]);
                if (audioMap["type"] == "asset")
                    audio = Audio::asset(audioMap["resource"]);
                AudioPlayer* audioPlayer = audioPlayers->get(id);
                audioPlayer->open(audio);
            }
            if (type == "playlist") {
                std::vector<Audio*> audios;
                int start = method->getArgument<int>("start");
                std::vector<std::map<std::string,std::string>> playlistMap = method->getArgument<std::vector<std::map<std::string,std::string>>>("playlist");
                for (std::map<std::string,std::string> audioMap: playlistMap) {
                    Audio* audio;
                    if (audioMap["type"] == "file")
                        audio = Audio::file(audioMap["resource"]);
                    if (audioMap["type"] == "network")
                        audio = Audio::network(audioMap["resource"]);
                    if (audioMap["type"] == "asset")
                        audio = Audio::asset(audioMap["resource"]);
                    audios.emplace_back(audio);
                }
                AudioPlayer* audioPlayer = audioPlayers->get(id);
                audioPlayer->open(
                    new Playlist(
                        audios,
                        start
                    )
                );
            }
            method->returnNull();
        }
        /*
         * Plays the [AudioPlayer] instance.
         * 
         * Argument:
         * 
         * {
         *      'id': 0
         * }
         * 
         */
        else if (method->name == "play") {
            int id = method->getArgument<int>("id");
            AudioPlayer* audioPlayer = audioPlayers->get(id);
            audioPlayer->play();
            method->returnNull();
        }
        /*
         * Pauses the [AudioPlayer] instance.
         * 
         * Argument:
         * 
         * {
         *      'id': 0
         * }
         * 
         */
        else if (method->name == "pause") {
            int id = method->getArgument<int>("id");
            AudioPlayer* audioPlayer = audioPlayers->get(id);
            audioPlayer->pause();
            method->returnNull();
        }
        /*
         * Stops the [AudioPlayer] instance.
         * 
         * Argument:
         * 
         * {
         *      'id': 0
         * }
         * 
         */
        else if (method->name == "stop") {
            int id = method->getArgument<int>("id");
            AudioPlayer* audioPlayer = audioPlayers->get(id);
            audioPlayer->stop();
            method->returnNull();
        }
        /*
         * Jumps to next audio in the [Playlist] loaded in [AudioPlayer] instance.
         * 
         * Argument:
         * 
         * {
         *      'id': 0
         * }
         * 
         */
        else if (method->name == "next") {
            int id = method->getArgument<int>("id");
            AudioPlayer* audioPlayer = audioPlayers->get(id);
            audioPlayer->next();
            method->returnNull();
        }
        /*
         * Jumps to previous audio in the [Playlist] loaded in [AudioPlayer] instance.
         * 
         * Argument:
         * 
         * {
         *      'id': 0
         * }
         * 
         */
        else if (method->name == "back") {
            int id = method->getArgument<int>("id");
            AudioPlayer* audioPlayer = audioPlayers->get(id);
            audioPlayer->back();
            method->returnNull();
        }
        /*
         * Jumps to specific index in the [Playlist] loaded in [AudioPlayer] instance.
         * 
         * Argument:
         * 
         * {
         *      'id': 0,
         *      'index': 5
         * }
         * 
         */
        else if (method->name == "jump") {
            int id = method->getArgument<int>("id");
            int index = method->getArgument<int>("index");
            AudioPlayer* audioPlayer = audioPlayers->get(id);
            audioPlayer->jump(index);
            method->returnNull();
        }
        /*
         * Seeks [AudioPlayer] instance.
         * 
         * Argument:
         * 
         * {
         *      'id': 0,
         *      'duration': 50000
         * }
         * 
         */
        else if (method->name == "seek") {
            int id = method->getArgument<int>("id");
            int duration = method->getArgument<int>("duration");
            AudioPlayer* audioPlayer = audioPlayers->get(id);
            audioPlayer->seek(duration);
            method->returnNull();
        }
        /*
         * Sets volume of [AudioPlayer] instance.
         * 
         * Argument:
         * 
         * {
         *      'id': 0,
         *      'volume': 0.5
         * }
         * 
         */
        else if (method->name == "setVolume") {
            int id = method->getArgument<int>("id");
            double volume = method->getArgument<double>("volume");
            AudioPlayer* audioPlayer = audioPlayers->get(id);
            audioPlayer->setVolume(volume);
            method->returnNull();
        }
        /*
         * Sets playback rate of [AudioPlayer] instance.
         * 
         * Argument:
         * 
         * {
         *      'id': 0,
         *      'rate': 1.2
         * }
         * 
         */
        else if (method->name == "setRate") {
            int id = method->getArgument<int>("id");
            double rate = method->getArgument<double>("rate");
            AudioPlayer* audioPlayer = audioPlayers->get(id);
            audioPlayer->setRate(rate);
            method->returnNull();
        }
        else {
            method->returnNotImplemented();
        }
        method->returnResult();
    }

}

void FlutterVlcPluginRegisterWithRegistrar(FlutterDesktopPluginRegistrarRef registrar) {
    FlutterVlcPlugin::RegisterWithRegistrar(flutter::PluginRegistrarManager::GetInstance()->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}