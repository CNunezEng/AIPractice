"""
Chat server implementation following AIinfo.txt standards.
Uses async/await for non-blocking operations and proper error handling.
"""

import asyncio
import json
import logging
from datetime import datetime
from typing import Dict, List
import websockets
from websockets.server import WebSocketServerProtocol

from ..interfaces import IChatServer, IMessageHandler, Message


class MessageHandler(IMessageHandler):
    """
    Concrete implementation of IMessageHandler.
    Follows strategy pattern for message processing.
    """
    
    def __init__(self):
        self.logger = logging.getLogger(__name__)
    
    async def handle_message(self, message: Message) -> None:
        """Process incoming message - currently just logs it."""
        self.logger.info(f"Processing message from {message.sender_id}: {message.content}")
        # Add message processing logic here (filtering, formatting, etc.)
    
    async def validate_message(self, message: Message) -> bool:
        """Validate message format and content."""
        if not message.sender_id or not message.content:
            return False
        if len(message.content) > 1000:  # Prevent overly long messages
            return False
        return True


class ChatServer(IChatServer):
    """
    Concrete chat server implementation.
    Uses dependency injection for message handler.
    Implements observer pattern for client management.
    """
    
    def __init__(self, message_handler: IMessageHandler = None):
        self.clients: Dict[str, WebSocketServerProtocol] = {}
        self.message_handler = message_handler or MessageHandler()
        self.server = None
        self.is_running = False
        
        # Set up logging
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        self.logger = logging.getLogger(__name__)
    
    async def start_server(self, host: str, port: int) -> None:
        """Start the chat server on specified host and port."""
        try:
            self.server = await websockets.serve(
                self.handle_client_connection,
                host,
                port
            )
            self.is_running = True
            self.logger.info(f"Chat server started on {host}:{port}")
        except Exception as e:
            self.logger.error(f"Failed to start server: {e}")
            raise
    
    async def stop_server(self) -> None:
        """Stop the chat server gracefully."""
        if self.server:
            self.server.close()
            await self.server.wait_closed()
            self.is_running = False
            self.logger.info("Chat server stopped")
    
    async def broadcast_message(self, message: Message) -> None:
        """Broadcast message to all connected clients."""
        if not await self.message_handler.validate_message(message):
            self.logger.warning(f"Invalid message rejected: {message}")
            return
        
        await self.message_handler.handle_message(message)
        
        # Send to all connected clients
        disconnected_clients = []
        for client_id, websocket in self.clients.items():
            try:
                await websocket.send(message.to_json())
                self.logger.debug(f"Message sent to {client_id}")
            except websockets.exceptions.ConnectionClosed:
                disconnected_clients.append(client_id)
            except Exception as e:
                self.logger.error(f"Error sending to {client_id}: {e}")
                disconnected_clients.append(client_id)
        
        # Clean up disconnected clients
        for client_id in disconnected_clients:
            await self.handle_client_disconnection(client_id)
    
    async def handle_client_connection(self, websocket: WebSocketServerProtocol, path: str) -> None:
        """Handle new client connection."""
        client_id = f"client_{len(self.clients) + 1}_{id(websocket)}"
        self.clients[client_id] = websocket
        
        self.logger.info(f"Client {client_id} connected")
        
        # Send welcome message
        welcome_message = Message(
            sender_id="server",
            content=f"Welcome {client_id}! You are now connected.",
            timestamp=datetime.now()
        )
        await websocket.send(welcome_message.to_json())
        
        # Notify other clients
        join_message = Message(
            sender_id="server",
            content=f"{client_id} has joined the chat.",
            timestamp=datetime.now()
        )
        await self.broadcast_message(join_message)
        
        try:
            async for message in websocket:
                try:
                    data = json.loads(message)
                    chat_message = Message.from_dict(data)
                    chat_message.sender_id = client_id  # Ensure correct sender
                    await self.broadcast_message(chat_message)
                except json.JSONDecodeError:
                    error_message = Message(
                        sender_id="server",
                        content="Invalid message format. Please send valid JSON.",
                        timestamp=datetime.now()
                    )
                    await websocket.send(error_message.to_json())
                except Exception as e:
                    self.logger.error(f"Error processing message from {client_id}: {e}")
        except websockets.exceptions.ConnectionClosed:
            pass
        finally:
            await self.handle_client_disconnection(client_id)
    
    async def handle_client_disconnection(self, client_id: str) -> None:
        """Handle client disconnection."""
        if client_id in self.clients:
            del self.clients[client_id]
            self.logger.info(f"Client {client_id} disconnected")
            
            # Notify other clients
            leave_message = Message(
                sender_id="server",
                content=f"{client_id} has left the chat.",
                timestamp=datetime.now()
            )
            await self.broadcast_message(leave_message)
    
    def get_connected_clients(self) -> List[str]:
        """Get list of currently connected client IDs."""
        return list(self.clients.keys())


# Factory function for dependency injection
def create_chat_server(message_handler: IMessageHandler = None) -> ChatServer:
    """
    Factory function to create chat server with dependency injection.
    Follows factory pattern for object creation.
    """
    return ChatServer(message_handler)
