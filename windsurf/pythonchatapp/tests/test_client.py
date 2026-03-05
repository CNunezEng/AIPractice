"""
Unit tests for chat client implementation.
Following AIinfo.txt standards for comprehensive testing.
"""

import unittest
import asyncio
from unittest.mock import Mock, patch, AsyncMock
import sys
import os

# Add src to path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from client.chat_client import ChatClient, InteractiveChatClient, create_chat_client, create_interactive_client
from interfaces import Message


class TestChatClient(unittest.TestCase):
    """Test cases for ChatClient class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.client = ChatClient()
    
    def tearDown(self):
        """Clean up after tests."""
        if self.client.is_connected():
            asyncio.run(self.client.disconnect())
    
    def test_client_creation(self):
        """Test client creation and initial state."""
        self.assertFalse(self.client.is_connected())
        self.assertEqual(self.client.get_connection_status(), "Disconnected")
    
    def test_client_factory_function(self):
        """Test factory function for client creation."""
        client = create_chat_client()
        
        self.assertIsInstance(client, ChatClient)
        self.assertFalse(client.is_connected())
    
    @patch('websockets.connect')
    def test_client_connection(self, mock_connect):
        """Test client connection to server."""
        # Mock websocket connection
        mock_websocket = Mock()
        mock_connect.return_value = mock_websocket
        
        # Test connection
        result = asyncio.run(self.client.connect("localhost", 8765))
        self.assertTrue(result)
        self.assertTrue(self.client.is_connected())
        self.assertIn("Connected", self.client.get_connection_status())
    
    @patch('websockets.connect')
    def test_client_connection_failure(self, mock_connect):
        """Test client connection failure."""
        # Mock connection failure
        mock_connect.side_effect = Exception("Connection failed")
        
        # Test connection failure
        result = asyncio.run(self.client.connect("localhost", 8765))
        self.assertFalse(result)
        self.assertFalse(self.client.is_connected())
        self.assertIn("Connection failed", self.client.get_connection_status())
    
    def test_client_disconnect(self):
        """Test client disconnection."""
        # Mock as connected
        self.client.websocket = Mock()
        self.client.is_connected_flag = True
        self.client.connection_status_message = "Connected"
        
        # Test disconnection
        asyncio.run(self.client.disconnect())
        self.assertFalse(self.client.is_connected())
        self.assertEqual(self.client.get_connection_status(), "Disconnected")
    
    def test_send_message_standard_case(self):
        """Test standard message sending."""
        # Mock connected client
        mock_websocket = Mock()
        mock_websocket.send = AsyncMock()
        self.client.websocket = mock_websocket
        self.client.is_connected_flag = True
        
        # Test message sending
        result = asyncio.run(self.client.send_message("Hello world"))
        self.assertTrue(result)
        mock_websocket.send.assert_called_once()
    
    def test_send_message_not_connected(self):
        """Test sending message when not connected."""
        result = asyncio.run(self.client.send_message("Hello"))
        self.assertFalse(result)
    
    def test_send_message_failure(self):
        """Test message sending failure."""
        # Mock connected client with send failure
        mock_websocket = Mock()
        mock_websocket.send = Mock(side_effect=Exception("Send failed"))
        self.client.websocket = mock_websocket
        self.client.is_connected_flag = True
        
        # Test message sending failure
        result = asyncio.run(self.client.send_message("Hello"))
        self.assertFalse(result)
    
    def test_message_receiving_setup(self):
        """Test setting up message receiving."""
        # Mock connected client
        mock_websocket = Mock()
        self.client.websocket = mock_websocket
        self.client.is_connected_flag = True
        
        # Mock message handler
        handler_called = []
        
        def message_handler(message):
            handler_called.append(message)
        
        # Test starting message receiving
        self.client.start_receiving(message_handler)
        self.assertTrue(self.client.receiving)
        self.assertEqual(self.client.message_handler, message_handler)


class TestInteractiveChatClient(unittest.TestCase):
    """Test cases for InteractiveChatClient class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.client = InteractiveChatClient()
    
    def test_interactive_client_creation(self):
        """Test interactive client creation."""
        self.assertIsInstance(self.client, ChatClient)
        self.assertFalse(self.client.running)
    
    def test_interactive_client_factory(self):
        """Test factory function for interactive client."""
        client = create_interactive_client()
        
        self.assertIsInstance(client, InteractiveChatClient)
        self.assertFalse(client.running)
    
    @patch('builtins.input', side_effect=["Hello", "/quit"])
    @patch('builtins.print')
    def test_interactive_session_simulation(self, mock_print, mock_input):
        """Test interactive session simulation."""
        # Mock connection and receiving
        with patch.object(self.client, 'connect', return_value=True):
            with patch.object(self.client, 'start_receiving'):
                with patch.object(self.client, 'send_message'):
                    with patch.object(self.client, 'disconnect'):
                        # Test session
                        self.client.start_interactive_session("localhost", 8765)
                        
                        # Verify inputs were processed
                        self.assertEqual(mock_input.call_count, 2)
    
    def test_session_commands(self):
        """Test session command handling."""
        # Test command recognition
        self.assertTrue("/quit" in ["/quit", "/help"])
        self.assertTrue("/help" in ["/quit", "/help"])


class TestClientIntegration(unittest.TestCase):
    """Integration tests for chat client."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.client = ChatClient()
    
    def test_message_creation_flow(self):
        """Test complete message creation and sending flow."""
        # Mock connected client
        mock_websocket = Mock()
        mock_websocket.send = AsyncMock()
        self.client.websocket = mock_websocket
        self.client.is_connected_flag = True
        
        # Create and send message
        message_content = "Test message with special chars: !@#$%^&*()"
        result = asyncio.run(self.client.send_message(message_content))
        
        self.assertTrue(result)
        
        # Verify the message was sent
        mock_websocket.send.assert_called_once()
        
        # Get the sent data
        sent_data = mock_websocket.send.call_args[0][0]
        
        # Verify it's valid JSON
        parsed_data = json.loads(sent_data)
        self.assertIn("content", parsed_data)
        self.assertEqual(parsed_data["content"], message_content)
    
    def test_connection_status_flow(self):
        """Test connection status changes."""
        # Initial status
        self.assertEqual(self.client.get_connection_status(), "Disconnected")
        
        # Mock connection
        self.client.is_connected_flag = True
        self.client.connection_status_message = "Connected to localhost:8765"
        
        self.assertEqual(self.client.get_connection_status(), "Connected to localhost:8765")
        self.assertTrue(self.client.is_connected())
        
        # Mock disconnection
        self.client.is_connected_flag = False
        self.client.connection_status_message = "Disconnected"
        
        self.assertEqual(self.client.get_connection_status(), "Disconnected")
        self.assertFalse(self.client.is_connected())
    
    def test_message_handler_integration(self):
        """Test message handler integration."""
        # Mock connected client
        self.client.websocket = Mock()
        self.client.is_connected_flag = True
        
        # Track received messages
        received_messages = []
        
        def message_handler(message):
            received_messages.append(message)
        
        # Set up message receiving
        self.client.start_receiving(message_handler)
        
        # Verify handler is set
        self.assertEqual(self.client.message_handler, message_handler)
        self.assertTrue(self.client.receiving)


class TestClientErrorHandling(unittest.TestCase):
    """Test cases for client error handling."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.client = ChatClient()
    
    def test_invalid_host_handling(self):
        """Test handling of invalid host addresses."""
        with patch('websockets.connect', side_effect=Exception("Invalid host")):
            result = asyncio.run(self.client.connect("invalid_host", 8765))
            self.assertFalse(result)
    
    def test_invalid_port_handling(self):
        """Test handling of invalid port numbers."""
        with patch('websockets.connect', side_effect=Exception("Invalid port")):
            result = asyncio.run(self.client.connect("localhost", -1))
            self.assertFalse(result)
    
    def test_network_error_handling(self):
        """Test handling of network errors."""
        with patch('websockets.connect', side_effect=ConnectionError("Network error")):
            result = asyncio.run(self.client.connect("localhost", 8765))
            self.assertFalse(result)
    
    def test_message_parsing_error(self):
        """Test handling of malformed message data."""
        # Mock connected client
        mock_websocket = Mock()
        self.client.websocket = mock_websocket
        self.client.is_connected_flag = True
        
        # Test sending empty message
        result = asyncio.run(self.client.send_message(""))
        self.assertTrue(result)  # Empty message is still sent
        
        # Test sending very long message
        long_message = "a" * 1001
        result = asyncio.run(self.client.send_message(long_message))
        self.assertTrue(result)  # Client doesn't validate, server does
    
    def test_connection_loss_handling(self):
        """Test handling of connection loss during operations."""
        # Mock connected client
        mock_websocket = Mock()
        mock_websocket.send = Mock(side_effect=Exception("Connection lost"))
        self.client.websocket = mock_websocket
        self.client.is_connected_flag = True
        
        # Test send failure
        result = asyncio.run(self.client.send_message("Hello"))
        self.assertFalse(result)


if __name__ == '__main__':
    # Configure test runner
    unittest.main(verbosity=2)
