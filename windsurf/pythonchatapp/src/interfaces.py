"""
Chat application interfaces and message protocol.
Following AIinfo.txt standards for explicit interfaces and design patterns.
"""

from abc import ABC, abstractmethod
from dataclasses import dataclass
from datetime import datetime
from typing import List, Optional
import json


@dataclass
class Message:
    """
    Message data structure for chat communication.
    Follows single responsibility principle - only contains message data.
    """
    sender_id: str
    content: str
    timestamp: datetime
    message_type: str = "text"  # Can be extended for different message types
    
    def to_dict(self) -> dict:
        """Convert message to dictionary for JSON serialization."""
        return {
            "sender_id": self.sender_id,
            "content": self.content,
            "timestamp": self.timestamp.isoformat(),
            "message_type": self.message_type
        }
    
    @classmethod
    def from_dict(cls, data: dict) -> 'Message':
        """Create message from dictionary."""
        return cls(
            sender_id=data["sender_id"],
            content=data["content"],
            timestamp=datetime.fromisoformat(data["timestamp"]),
            message_type=data.get("message_type", "text")
        )
    
    def to_json(self) -> str:
        """Convert message to JSON string."""
        return json.dumps(self.to_dict())
    
    @classmethod
    def from_json(cls, json_str: str) -> 'Message':
        """Create message from JSON string."""
        return cls.from_dict(json.loads(json_str))


class IChatServer(ABC):
    """
    Interface for chat server implementation.
    Uses dependency injection pattern for client handlers.
    """
    
    @abstractmethod
    async def start_server(self, host: str, port: int) -> None:
        """Start the chat server on specified host and port."""
        pass
    
    @abstractmethod
    async def stop_server(self) -> None:
        """Stop the chat server gracefully."""
        pass
    
    @abstractmethod
    async def broadcast_message(self, message: Message) -> None:
        """Broadcast message to all connected clients."""
        pass
    
    @abstractmethod
    async def handle_client_connection(self, client_id: str, connection) -> None:
        """Handle new client connection."""
        pass
    
    @abstractmethod
    async def handle_client_disconnection(self, client_id: str) -> None:
        """Handle client disconnection."""
        pass
    
    @abstractmethod
    def get_connected_clients(self) -> List[str]:
        """Get list of currently connected client IDs."""
        pass


class IChatClient(ABC):
    """
    Interface for chat client implementation.
    Follows interface segregation principle - only chat-related methods.
    """
    
    @abstractmethod
    async def connect(self, host: str, port: int) -> None:
        """Connect to chat server."""
        pass
    
    @abstractmethod
    async def disconnect(self) -> None:
        """Disconnect from chat server."""
        pass
    
    @abstractmethod
    async def send_message(self, content: str) -> None:
        """Send message to server."""
        pass
    
    @abstractmethod
    async def receive_messages(self) -> None:
        """Start receiving messages from server."""
        pass
    
    @abstractmethod
    def is_connected(self) -> bool:
        """Check if client is connected to server."""
        pass
    
    @abstractmethod
    def get_connection_status(self) -> str:
        """Get detailed connection status."""
        pass


class IMessageHandler(ABC):
    """
    Interface for message handling strategies.
    Allows for different message processing implementations.
    """
    
    @abstractmethod
    async def handle_message(self, message: Message) -> None:
        """Process incoming message."""
        pass
    
    @abstractmethod
    async def validate_message(self, message: Message) -> bool:
        """Validate message format and content."""
        pass
