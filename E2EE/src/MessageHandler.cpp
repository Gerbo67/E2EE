#include "MessageHandler.h"
#include "crow.h"

void MessageHandler::process(crow::websocket::connection& from_conn, const std::string& data) {
    auto& userManager = UserManager::getInstance();
    auto sender_user = userManager.findUserByConnection(&from_conn);

    if (!sender_user) return;

    crow::json::rvalue msg;
    try {
        msg = crow::json::load(data);
    } catch (...) {
        sender_user->sendMessage(buildErrorMessage("Formato de mensaje inválido."));
        return;
    }

    std::string type;
    if (msg.has("type")) {
        type = msg["type"].s();
    }

    if (type == "register_key") {
        if (msg.has("publicKey")) {
            std::string pk = msg["publicKey"].s();
            sender_user->setPublicKey(pk);
            sender_user->sendMessage(buildWelcomeMessage(sender_user->getUsername()));
            sender_user->sendMessage(buildUserListWithKeysMessage(userManager.getAllUsersInfo()));
            userManager.broadcast(buildUserStatusUpdate(sender_user->getUsername(), sender_user->getPublicKey(), "online"), &from_conn);
        }
        return;
    }

    if (sender_user->getPublicKey().empty()) {
        sender_user->sendMessage(buildKeyRequestMessage());
        return;
    }

    if (type == "private_encrypted") {
        if (!msg.has("recipient")) return;
        std::string recipient_name = msg["recipient"].s();
        auto recipient_user = userManager.findUserByUsername(recipient_name);
        if (recipient_user) {
            recipient_user->sendMessage(buildForwardedEncryptedMessage(sender_user->getUsername(), msg));
        }
    } else if (type == "public_encrypted") {
        userManager.broadcast(buildForwardedPublicEncryptedMessage(sender_user->getUsername(), msg), &from_conn);
    } else if (type == "start_typing" || type == "stop_typing") {
        if (!msg.has("recipient")) return;
        std::string recipient_name = msg["recipient"].s();
        
        std::string response_message;
        if (type == "start_typing") {
            response_message = buildUserTypingMessage(sender_user->getUsername(), recipient_name);
        } else {
            response_message = buildUserStoppedTypingMessage(sender_user->getUsername(), recipient_name);
        }

        if (recipient_name == "Sala General") {
            userManager.broadcast(response_message, &from_conn);
        } else {
            auto recipient_user = userManager.findUserByUsername(recipient_name);
            if (recipient_user) {
                recipient_user->sendMessage(response_message);
            }
        }
    }
}

// --- Implementaciones de los constructores (con los nuevos métodos) ---

std::string MessageHandler::buildKeyRequestMessage() {
    crow::json::wvalue msg;
    msg["type"] = "request_key";
    return msg.dump();
}

std::string MessageHandler::buildWelcomeMessage(const std::string& username) {
    crow::json::wvalue msg;
    msg["type"] = "welcome";
    msg["username"] = username;
    return msg.dump();
}

std::string MessageHandler::buildUserListWithKeysMessage(const std::vector<UserManager::UserInfo>& users) {
    crow::json::wvalue msg;
    msg["type"] = "user_list";
    crow::json::wvalue::list user_list;
    for (const auto& info : users) {
        crow::json::wvalue user_obj;
        user_obj["username"] = info.username;
        user_obj["publicKey"] = info.publicKey;
        user_obj["status"] = "online";
        user_list.push_back(std::move(user_obj));
    }
    msg["users"] = std::move(user_list);
    return msg.dump();
}

std::string MessageHandler::buildUserStatusUpdate(const std::string& username, const std::string& publicKey, const std::string& status) {
    crow::json::wvalue msg;
    msg["type"] = "user_status";
    msg["username"] = username;
    msg["publicKey"] = publicKey;
    msg["status"] = status;
    return msg.dump();
}

std::string MessageHandler::buildForwardedEncryptedMessage(const std::string& sender, const crow::json::rvalue& original_message) {
    crow::json::wvalue msg;
    msg["type"] = "private_encrypted";
    msg["sender"] = sender;
    msg["key"] = original_message["key"].s();
    msg["iv"] = original_message["iv"].s();
    msg["payload"] = original_message["payload"].s();
    return msg.dump();
}

std::string MessageHandler::buildForwardedPublicEncryptedMessage(const std::string& sender, const crow::json::rvalue& original_message) {
    crow::json::wvalue msg;
    msg["type"] = "public_encrypted";
    msg["sender"] = sender;
    msg["iv"] = original_message["iv"].s();
    msg["payload"] = original_message["payload"].s();
    
    crow::json::wvalue keys_obj;
    for(const auto& key : original_message["keys"]){
        keys_obj[key.key()] = key.s();
    }
    msg["keys"] = std::move(keys_obj);
    
    return msg.dump();
}

std::string MessageHandler::buildUserTypingMessage(const std::string& sender, const std::string& recipient) {
    crow::json::wvalue msg;
    msg["type"] = "user_typing";
    msg["sender"] = sender;
    msg["recipient"] = recipient;
    return msg.dump();
}

std::string MessageHandler::buildUserStoppedTypingMessage(const std::string& sender, const std::string& recipient) {
    crow::json::wvalue msg;
    msg["type"] = "user_stopped_typing";
    msg["sender"] = sender;
    msg["recipient"] = recipient;
    return msg.dump();
}

std::string MessageHandler::buildErrorMessage(const std::string& error_text) {
    crow::json::wvalue msg;
    msg["type"] = "error";
    msg["message"] = error_text;
    return msg.dump();
}
