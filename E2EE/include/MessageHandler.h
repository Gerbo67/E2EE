#pragma once

#include "UserManager.h"
#include <string>
#include <vector>

/**
 * @class MessageHandler
 * @brief Maneja el procesamiento y construcción de mensajes del protocolo WebSocket.
 *
 * Esta clase se encarga de procesar todos los mensajes entrantes del cliente,
 * validar su formato JSON, y construir las respuestas apropiadas para el
 * protocolo de chat cifrado de extremo a extremo.
 */
class MessageHandler {
public:
    /**
     * @brief Procesa un mensaje entrante de un cliente WebSocket.
     * @param from_conn Referencia a la conexión del remitente.
     * @param data Datos del mensaje en formato JSON.
     */
    static void process(crow::websocket::connection& from_conn, const std::string& data);

    /**
     * @brief Construye un mensaje de solicitud de clave pública.
     * @return Mensaje JSON solicitando la clave pública del cliente.
     */
    static std::string buildKeyRequestMessage();
    
    /**
     * @brief Construye un mensaje de bienvenida para un usuario registrado.
     * @param username Nombre del usuario que se registró.
     * @return Mensaje JSON de bienvenida.
     */
    static std::string buildWelcomeMessage(const std::string& username);
    
    /**
     * @brief Construye un mensaje con la lista de usuarios y sus claves públicas.
     * @param users Vector con información de todos los usuarios conectados.
     * @return Mensaje JSON con la lista de usuarios.
     */
    static std::string buildUserListWithKeysMessage(const std::vector<UserManager::UserInfo>& users);
    
    /**
     * @brief Construye un mensaje de actualización de estado de usuario.
     * @param username Nombre del usuario.
     * @param publicKey Clave pública del usuario.
     * @param status Estado del usuario (online/offline).
     * @return Mensaje JSON de actualización de estado.
     */
    static std::string buildUserStatusUpdate(const std::string& username, const std::string& publicKey, const std::string& status);
    
    /**
     * @brief Construye un mensaje cifrado privado para reenvío.
     * @param sender Nombre del remitente.
     * @param original_message Mensaje original del cliente.
     * @return Mensaje JSON cifrado para un destinatario específico.
     */
    static std::string buildForwardedEncryptedMessage(const std::string& sender, const crow::json::rvalue& original_message);
    
    /**
     * @brief Construye un mensaje cifrado público para reenvío.
     * @param sender Nombre del remitente.
     * @param original_message Mensaje original del cliente.
     * @return Mensaje JSON cifrado para múltiples destinatarios.
     */
    static std::string buildForwardedPublicEncryptedMessage(const std::string& sender, const crow::json::rvalue& original_message);
    
    /**
     * @brief Construye un mensaje para notificar que un usuario está escribiendo.
     * @param sender Usuario que está escribiendo.
     * @param recipient Destinatario del mensaje.
     * @return Mensaje JSON de notificación de escritura.
     */
    static std::string buildUserTypingMessage(const std::string& sender, const std::string& recipient);
    
    /**
     * @brief Construye un mensaje para notificar que un usuario dejó de escribir.
     * @param sender Usuario que dejó de escribir.
     * @param recipient Destinatario del mensaje.
     * @return Mensaje JSON de fin de notificación de escritura.
     */
    static std::string buildUserStoppedTypingMessage(const std::string& sender, const std::string& recipient);

    /**
     * @brief Construye un mensaje de error para enviar al cliente.
     * @param error_text Descripción del error ocurrido.
     * @return Mensaje JSON de error.
     */
    static std::string buildErrorMessage(const std::string& error_text);
};
