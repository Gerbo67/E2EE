#pragma once

#include "UserManager.h"
#include <string>
#include <vector>

class MessageHandler {
public:
    static void process(crow::websocket::connection& from_conn, const std::string& data);

    // --- Constructores de Mensajes para el Servidor ---
    static std::string buildKeyRequestMessage();
    static std::string buildWelcomeMessage(const std::string& username);
    static std::string buildUserListWithKeysMessage(const std::vector<UserManager::UserInfo>& users);
    static std::string buildUserStatusUpdate(const std::string& username, const std::string& publicKey, const std::string& status);
    static std::string buildForwardedEncryptedMessage(const std::string& sender, const crow::json::rvalue& original_message);
    static std::string buildForwardedPublicEncryptedMessage(const std::string& sender, const crow::json::rvalue& original_message);
    
    /**
     * @brief Construye un mensaje para notificar que un usuario está escribiendo.
     */
    static std::string buildUserTypingMessage(const std::string& sender, const std::string& recipient);
    
    /**
     * @brief Construye un mensaje para notificar que un usuario dejó de escribir.
     */
    static std::string buildUserStoppedTypingMessage(const std::string& sender, const std::string& recipient);

    static std::string buildErrorMessage(const std::string& error_text);
};
