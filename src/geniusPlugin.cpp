#include "fpp-pch.h"

#include <string>
#include <memory>

#include <httpserver.hpp>
#include "common.h"
#include "settings.h"
#include "Plugin.h"
#include "Plugins.h"
#include "log.h"
#include "MultiSync.h"

#include "fppversion_defines.h"

#include "commands/Commands.h"

#include "genius_controller.h"

class GeniusPlugin : public FPPPlugin, public httpserver::http_resource {
private:

public:
    GeniusPlugin() : FPPPlugin("fpp-plugin-GeniusPlugin") {
        LogInfo(VB_PLUGIN, "Initializing Genius Plugin\n");
        //readFiles();
        registerCommand();
    }
    virtual ~GeniusPlugin() {

    }

    class GeniusGetControllerStatus : public Command {
    public:
        GeniusGetControllerStatus(GeniusPlugin *p) : Command("Get Genius Controller Status",
        "Get Genius Controller Status"), plugin(p) {
            args.push_back(CommandArg("IP", "string", "IP Address"));
        }
        
        virtual std::unique_ptr<Command::Result> run(const std::vector<std::string> &args) override {
            std::string ipAddress = "";
            //plugin->SetControllerTestModeOn(ipAddress, type, checkList);
            return std::make_unique<Command::Result>("Get Genius Controller Status");
        }
        GeniusPlugin *plugin;
    };      

    class GeniusGetAllControllerStatus : public Command {
    public:
        GeniusGetAllControllerStatus(GeniusPlugin *p) : Command("Get All Genius Controller Status",
        "Get All Genius Controller Status using Multisync List"), plugin(p) {
        }
        
        virtual std::unique_ptr<Command::Result> run(const std::vector<std::string> &args) override {                        
            //plugin->SetAllControllerTestModeOff();
            return std::make_unique<Command::Result>("Get Genius Controller Status");
        }
        GeniusPlugin *plugin;
    };


    void registerCommand() {
        CommandManager::INSTANCE.addCommand(new GeniusGetControllerStatus(this));
        CommandManager::INSTANCE.addCommand(new GeniusGetAllControllerStatus(this));
    }

    std::unique_ptr<GeniusController> MakeController(std::string const& ip, MultiSyncSystemType const& type) {
        if (0xA0 <= type && 0xAF > type) {
            return std::move(std::make_unique<GeniusController>(ip));
        } else {
            LogInfo(VB_PLUGIN, "controller type not found '%d'\n", type);
        }
        return nullptr;
    }

    void SetControllerTestModeOn(std::string const& ip) {
        if(ip.find(",") != std::string::npos) {
            auto ips = split(ip, ',');
            for(auto const& ip_ : ips) {
                std::unique_ptr<GeniusController> controllerItem = std::make_unique<GeniusController>(ip_);
                //if(controllerItem)controllerItem->setTestModeOn();
            }
        } else {
            std::unique_ptr<GeniusController> controllerItem = std::make_unique<GeniusController>(ip);
            //if(controllerItem)controllerItem->setTestModeOn();
        }
    }

    void SetAllControllerTestModeOn() {        
        for (auto system : multiSync->GetLocalSystems()){
            if(system.type == MultiSyncSystemType::kSysTypeUnknown) {
                return;
            }
            std::unique_ptr<GeniusController> controllerItem = MakeController(system.address, system.type);
            //if(controllerItem)controllerItem->setTestModeOn();
        }
        for (auto system : multiSync->GetRemoteSystems()){
            if(system.type == MultiSyncSystemType::kSysTypeUnknown) {
                return;
            }
            std::unique_ptr<GeniusController> controllerItem = MakeController(system.address, system.type);
            //if(controllerItem)controllerItem->setTestModeOn();
        }
    }

    bool IsInMultiSyncList(std::string const& ip)
    {
        for (auto system : multiSync->GetLocalSystems()){
            if(system.address == ip){
                return true;
            }
        }
        for (auto system : multiSync->GetRemoteSystems()){
            if(system.address == ip){
                 return true;
            }
        }
        LogInfo(VB_PLUGIN, "IP Address not found in Multisync local list '%s'\n", ip.c_str());
        return false;
    }

    MultiSyncSystemType GetMultiSyncType(std::string const& ip)
    {
        for (auto system : multiSync->GetLocalSystems()){
            if(system.address == ip){
                return system.type;
            }
        }
        for (auto system : multiSync->GetRemoteSystems()){
            if(system.address == ip){
                return system.type;
            }
        }
        LogInfo(VB_PLUGIN, "IP Address not found in Multisync local list '%s'\n", ip.c_str());
        return MultiSyncSystemType::kSysTypeUnknown;
    }
};

extern "C" {
    FPPPlugin *createPlugin() {
        return new GeniusPlugin();
    }
}
