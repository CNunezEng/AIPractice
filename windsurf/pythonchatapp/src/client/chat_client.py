"""
Chat client implementation following AIinfo.txt standards.
Uses async/await for non-blocking operations and proper error handling.
"""

import asyncio
import json
import logging
from datetime import datetime
from typing import Optional
import websockets
from websockets.client import WebSocketClientProtocol

from ..interfaces import IChatClient, Message


class ChatClient(IChatClient):
    """
    Concrete chat client implementation.
    Follows single responsibility principle - only handles chat functionality.
    """
    
    def __init__(self):
        self.websocket: Optional[WebSocketClientProtocol] = None
        self.is_connected_flag = False
        self.connection_status_message = "Disconnected"
        
        # Set up logging
        logging.basicConfig(
            level=logging.INFO,
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        self.logger = logging.getLogger(__name__)
    
    async def connect(self, host: str, port: int) -> None:
        """Connect to chat server."""
        try:
            uri = f"ws://{host}:{port}"
            self.websocket = await websockets.connect(uri)
            self.is_connected_flag = True
            self.connection_status_message = f"Connected to {host}:{port}"
            self.logger.info(f"Connected to chat server at {uri}")
        except Exception as e:
            self.is_connected_flag = False
            self.connection_status_message = f"Connection failed: {e}"
            self.logger.error(f"Failed to connect to server: {e}")
            raise
    
    async def disconnect(self) -> None:
        """Disconnect from chat server."""
        if self.websocket:
            try:
                await self.websocket.close()
                self.logger.info("Disconnected from server")
            except Exception as e:
                self.logger.error(f"Error during disconnect: {e}")
            finally:
                self.websocket = None
                self.is_connected_flag = False
                self.connection_status_message = "Disconnected"
    
    async def send_message(self, content: str) -> None:
        """Send message to server."""
        if not self.is_connected_flag or not self.websocket:
            raise ConnectionError("Not connected to server")
        
        try:
            message = Message(
                sender_id="client",  # Will be updated by server
                content=content,
                timestamp=datetime.now()
            )
            await self.websocket.send(message.to_json())
            self.logger.debug(f"Message sent: {content}")
        except Exception as e:
            self.logger.error(f"Failed to send message: {e}")
            raise
    
    async def receive_messages(self) -> None:
        """Start receiving messages from server."""
        if not self.is_connected_flag or not self.websocket:
            raise ConnectionError("Not connected to server")
        
        try:
            async for message in self.websocket:
                try:
                    data = json.loads(message)
                    chat_message = Message.from_dict(data)
                    await self.handle_received_message(chat_message)
                except json.JSONDecodeError:
                    self.logger.error("Received invalid JSON message")
                except Exception as e:
                    self.logger.error(f"Error processing received message: {e}")
        except websockets.exceptions.ConnectionClosed:
            self.logger.info("Server connection closed")
            self.is_connected_flag = False
            self.connection_status_message = "Connection lost"
        except Exception as e:
            self.logger.error(f"Error in receive loop: {e}")
            self.is_connected_flag = False
            self.connection_status_message = f"Error: {e}"
    
    async def handle_received_message(self, message: Message) -> None:
        """Handle received message - can be overridden for UI integration."""
        timestamp_str = message.timestamp.strftime("%H:%M:%S")
        print(f"[{timestamp_str}] {message.sender_id}: {message.content}")
    
    def is_connected(self) -> bool:
        """Check if client is connected to server."""
        return self.is_connected_flag
    
    def get_connection_status(self) -> str:
        """Get detailed connection status."""
        return self.connection_status_message


class InteractiveChatClient(ChatClient):
    """
    Interactive chat client with command line interface.
    Extends base client with user interaction capabilities.
    """
    
    def __init__(self):
        super().__init__()
        self.receive_task: Optional[asyncio.Task] = None
        self.running = False
    
    async def start_interactive_session(self, host: str, port: int) -> None:
        """Start interactive chat session."""
        try:
            await self.connect(host, port)
            self.running = True
            
            # Start receiving messages in background
            self.receive_task = asyncio.create_task(self.receive_messages())
            
            print("Connected to chat server! Type '/quit' to exit.")
            print("Type your message and press Enter to send.")
            
            # Main input loop
            while self.running:
                try:
                    content = await asyncio.get_event_loop().run_in_executor(
                        None, input, "> "
                    )
                    
                    if content.lower() == '/quit':
                        break
                    
                    if content.strip():
                        await self.send_message(content)
                
                except KeyboardInterrupt:
                    break
                except EOFError:
                    break
                except Exception as e:
                    self.logger.error(f"Error in input loop: {e}")
        
        finally:
            await self.stop_session()
    
    async def stop_session(self) -> None:
        """Stop the interactive session."""
        self.running = False
        
        if self.receive_task:
            self.receive_task.cancel()
            try:
                await self.receive_task
            except asyncio.CancelledError:
                pass
        
        await self.disconnect()
        print("Chat session ended.")


# Factory function for dependency injection
def create_chat_client() -> ChatClient:
    """
    Factory function to create chat client.
    Follows factory pattern for object creation.
    """
    return ChatClient()


def create_interactive_client() -> InteractiveChatClient:
    """
    Factory function to create interactive chat client.
    """
    return InteractiveChatClient()
