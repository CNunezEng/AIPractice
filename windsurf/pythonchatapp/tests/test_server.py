"""
Unit tests for chat server implementation.
Following AIinfo.txt standards for comprehensive testing.
"""

import unittest
import asyncio
import json
import websockets
from unittest.mock import Mock, patch, AsyncMock
import sys
import os

# Add src to path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from server.chat_server import ChatServer, MessageHandler, create_chat_server
from interfaces import Message


class TestMessageHandler(unittest.TestCase):
    """Test cases for MessageHandler class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.handler = MessageHandler()
    
    def test_message_handling_standard_case(self):
        """Test standard message handling."""
        message = Message("user1", "Hello world")
        
        # Test validation
        result = asyncio.run(self.handler.validate_message(message))
        self.assertTrue(result)
        
        # Test handling (should not raise exception)
        asyncio.run(self.handler.handle_message(message))
    
    def test_message_handling_edge_cases(self):
        """Test edge cases for message handling."""
        # Long message
        long_message = Message("user", "a" * 1000)
        result = asyncio.run(self.handler.validate_message(long_message))
        self.assertTrue(result)
        
        # Special characters
        special_message = Message("user", "Special chars: !@#$%^&*()_+-=[]{}|;':\",./<>?")
        result = asyncio.run(self.handler.validate_message(special_message))
        self.assertTrue(result)
    
    def test_message_handling_wrong_inputs(self):
        """Test wrong inputs for message handling."""
        # Empty message
        empty_message = Message("", "")
        result = asyncio.run(self.handler.validate_message(empty_message))
        self.assertFalse(result)
        
        # Too long message
        too_long_message = Message("user", "a" * 1001)
        result = asyncio.run(self.handler.validate_message(too_long_message))
        self.assertFalse(result)


class TestChatServer(unittest.TestCase):
    """Test cases for ChatServer class."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.server = ChatServer()
    
    def tearDown(self):
        """Clean up after tests."""
        if self.server.is_running:
            asyncio.run(self.server.stop_server())
    
    def test_server_creation(self):
        """Test server creation and initial state."""
        self.assertFalse(self.server.is_running)
        self.assertEqual(len(self.server.get_connected_clients()), 0)
    
    def test_server_with_custom_handler(self):
        """Test server creation with custom message handler."""
        custom_handler = MessageHandler()
        server = ChatServer(custom_handler)
        
        self.assertEqual(server.message_handler, custom_handler)
        self.assertFalse(server.is_running)
    
    def test_server_factory_function(self):
        """Test factory function for server creation."""
        server = create_chat_server()
        
        self.assertIsInstance(server, ChatServer)
        self.assertFalse(server.is_running)
    
    @patch('websockets.serve')
    def test_server_start_stop(self, mock_serve):
        """Test server start and stop functionality."""
        # Mock the websocket server
        mock_server_instance = Mock()
        mock_serve.return_value = mock_server_instance
        
        # Test starting server
        result = asyncio.run(self.server.start_server("localhost", 8765))
        self.assertTrue(result)
        self.assertTrue(self.server.is_running)
        
        # Test stopping server
        asyncio.run(self.server.stop_server())
        self.assertFalse(self.server.is_running)
    
    def test_broadcast_message_standard_case(self):
        """Test standard message broadcasting."""
        message = Message("user1", "Hello everyone")
        
        # Mock the server as running
        self.server.is_running = True
        
        # Test broadcasting (should not raise exception)
        result = asyncio.run(self.server.broadcast_message(message))
        self.assertTrue(result)
    
    def test_broadcast_message_edge_cases(self):
        """Test edge cases for message broadcasting."""
        # Server not running
        self.server.is_running = False
        message = Message("user1", "Hello")
        result = asyncio.run(self.server.broadcast_message(message))
        self.assertFalse(result)
        
        # Invalid message
        self.server.is_running = True
        invalid_message = Message("", "")
        result = asyncio.run(self.server.broadcast_message(invalid_message))
        self.assertFalse(result)
    
    def test_client_management(self):
        """Test client connection management."""
        # Initially no clients
        clients = self.server.get_connected_clients()
        self.assertEqual(len(clients), 0)
        
        # Mock client connections
        self.server.clients = {
            "client1": Mock(),
            "client2": Mock()
        }
        
        clients = self.server.get_connected_clients()
        self.assertEqual(len(clients), 2)
        self.assertIn("client1", clients)
        self.assertIn("client2", clients)


class TestChatServerIntegration(unittest.TestCase):
    """Integration tests for chat server."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.server = ChatServer()
    
    def tearDown(self):
        """Clean up after tests."""
        if self.server.is_running:
            asyncio.run(self.server.stop_server())
    
    def test_message_validation_flow(self):
        """Test complete message validation and broadcasting flow."""
        # Set up server as running
        self.server.is_running = True
        
        # Create valid message
        message = Message("user1", "Test message")
        
        # Test validation and broadcasting
        result = asyncio.run(self.server.broadcast_message(message))
        self.assertTrue(result)
    
    def test_multiple_message_types(self):
        """Test handling of different message types."""
        self.server.is_running = True
        
        message_types = ["text", "image", "file", "system"]
        
        for msg_type in message_types:
            message = Message("user", f"Content for {msg_type}", msg_type)
            result = asyncio.run(self.server.broadcast_message(message))
            self.assertTrue(result)
    
    def test_concurrent_client_simulation(self):
        """Test simulation of concurrent client connections."""
        import threading
        import time
        
        # Mock client connections
        clients = {}
        lock = threading.Lock()
        
        def add_client(client_id):
            with lock:
                clients[client_id] = Mock()
                time.sleep(0.01)  # Simulate connection time
        
        # Add multiple clients concurrently
        threads = []
        for i in range(10):
            thread = threading.Thread(target=add_client, args=(f"client_{i}",))
            threads.append(thread)
            thread.start()
        
        # Wait for all threads
        for thread in threads:
            thread.join()
        
        # Verify all clients were added
        self.assertEqual(len(clients), 10)
        
        # Update server clients
        self.server.clients = clients
        connected_clients = self.server.get_connected_clients()
        self.assertEqual(len(connected_clients), 10)


class TestServerErrorHandling(unittest.TestCase):
    """Test cases for server error handling."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.server = ChatServer()
    
    def test_invalid_port_handling(self):
        """Test handling of invalid port numbers."""
        with patch('websockets.serve', side_effect=Exception("Invalid port")):
            result = asyncio.run(self.server.start_server("localhost", -1))
            self.assertFalse(result)
    
    def test_network_error_handling(self):
        """Test handling of network errors."""
        with patch('websockets.serve', side_effect=ConnectionError("Network error")):
            result = asyncio.run(self.server.start_server("localhost", 8765))
            self.assertFalse(result)
    
    def test_message_broadcast_errors(self):
        """Test error handling during message broadcasting."""
        self.server.is_running = True
        
        # Mock client that raises exception during send
        mock_client = Mock()
        mock_client.send = Mock(side_effect=Exception("Connection lost"))
        self.server.clients = {"client1": mock_client}
        
        message = Message("user1", "Test message")
        
        # Should handle error gracefully
        result = asyncio.run(self.server.broadcast_message(message))
        self.assertTrue(result)  # Still returns True as error is handled internally


if __name__ == '__main__':
    # Configure test runner
    unittest.main(verbosity=2)
