"""
Unit tests for chat application interfaces and message protocol.
Following AIinfo.txt standards for comprehensive testing.
"""

import unittest
import json
from datetime import datetime
import sys
import os

# Add src to path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from interfaces import Message, IChatServer, IChatClient, IMessageHandler


class TestMessage(unittest.TestCase):
    """Test cases for Message class following standard/edge/wrong input pattern."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.valid_message = Message("user1", "Hello world", "text")
        self.test_time = datetime.now()
    
    def test_message_creation_standard_case(self):
        """Test standard message creation."""
        message = Message("user1", "Hello world")
        
        self.assertEqual(message.sender_id, "user1")
        self.assertEqual(message.content, "Hello world")
        self.assertEqual(message.message_type, "text")
        self.assertIsInstance(message.timestamp, datetime)
    
    def test_message_creation_edge_cases(self):
        """Test edge cases for message creation."""
        # Empty strings
        message1 = Message("", "content")
        self.assertEqual(message1.sender_id, "")
        self.assertEqual(message1.content, "content")
        
        # Long content
        long_content = "a" * 1000
        message2 = Message("user", long_content)
        self.assertEqual(len(message2.content), 1000)
        
        # Different message types
        message3 = Message("user", "content", "image")
        self.assertEqual(message3.message_type, "image")
    
    def test_message_serialization(self):
        """Test JSON serialization and deserialization."""
        # Test to_json
        json_str = self.valid_message.to_json()
        self.assertIsInstance(json_str, str)
        
        # Parse JSON to verify structure
        data = json.loads(json_str)
        self.assertIn("sender_id", data)
        self.assertIn("content", data)
        self.assertIn("timestamp", data)
        self.assertIn("message_type", data)
    
    def test_message_deserialization(self):
        """Test JSON deserialization."""
        json_str = self.valid_message.to_json()
        deserialized = Message.from_json(json_str)
        
        self.assertEqual(deserialized.sender_id, self.valid_message.sender_id)
        self.assertEqual(deserialized.content, self.valid_message.content)
        self.assertEqual(deserialized.message_type, self.valid_message.message_type)
    
    def test_message_invalid_inputs(self):
        """Test handling of invalid inputs."""
        # Invalid JSON
        with self.assertRaises(json.JSONDecodeError):
            Message.from_json("invalid json")
        
        # Missing fields in JSON
        incomplete_json = '{"sender_id": "user"}'
        message = Message.from_json(incomplete_json)
        self.assertEqual(message.sender_id, "user")
        self.assertEqual(message.content, "")  # Default empty


class MockMessageHandler(IMessageHandler):
    """Mock implementation of IMessageHandler for testing."""
    
    def __init__(self):
        self.processed_messages = []
        self.validation_results = []
    
    async def handle_message(self, message: Message) -> None:
        """Mock message handling."""
        self.processed_messages.append(message)
    
    async def validate_message(self, message: Message) -> bool:
        """Mock message validation."""
        result = message.is_valid()
        self.validation_results.append((message, result))
        return result


class TestMessageHandler(unittest.TestCase):
    """Test cases for message handler implementation."""
    
    def setUp(self):
        """Set up test fixtures."""
        self.handler = MockMessageHandler()
    
    def test_message_validation_standard_case(self):
        """Test standard message validation."""
        valid_message = Message("user1", "Hello world")
        result = valid_message.is_valid()
        
        self.assertTrue(result)
    
    def test_message_validation_edge_cases(self):
        """Test edge cases for message validation."""
        # Empty sender ID
        message1 = Message("", "content")
        self.assertFalse(message1.is_valid())
        
        # Empty content
        message2 = Message("user", "")
        self.assertFalse(message2.is_valid())
        
        # Maximum length content
        message3 = Message("user", "a" * 1000)
        self.assertTrue(message3.is_valid())
        
        # Too long content
        message4 = Message("user", "a" * 1001)
        self.assertFalse(message4.is_valid())
    
    def test_message_validation_wrong_inputs(self):
        """Test wrong inputs for message validation."""
        # None values (should be handled gracefully)
        message = Message("user", "content")
        message.sender_id = None
        self.assertFalse(message.is_valid())
        
        message.sender_id = "user"
        message.content = None
        self.assertFalse(message.is_valid())


class TestInterfaces(unittest.TestCase):
    """Test cases for interface definitions."""
    
    def test_interface_definitions(self):
        """Test that interfaces are properly defined."""
        # Test that interfaces cannot be instantiated directly
        with self.assertRaises(TypeError):
            IChatServer()
        
        with self.assertRaises(TypeError):
            IChatClient()
        
        with self.assertRaises(TypeError):
            IMessageHandler()
    
    def test_interface_method_existence(self):
        """Test that interface methods are defined."""
        # Check IChatServer methods
        server_methods = [
            'start_server', 'stop_server', 'broadcast_message',
            'handle_client_connection', 'handle_client_disconnection',
            'get_connected_clients'
        ]
        
        for method in server_methods:
            self.assertTrue(hasattr(IChatServer, method))
        
        # Check IChatClient methods
        client_methods = [
            'connect', 'disconnect', 'send_message',
            'receive_messages', 'is_connected', 'get_connection_status'
        ]
        
        for method in client_methods:
            self.assertTrue(hasattr(IChatClient, method))
        
        # Check IMessageHandler methods
        handler_methods = ['handle_message', 'validate_message']
        
        for method in handler_methods:
            self.assertTrue(hasattr(IMessageHandler, method))


class TestMessageIntegration(unittest.TestCase):
    """Integration tests for message handling."""
    
    def test_message_roundtrip(self):
        """Test complete message serialization/deserialization roundtrip."""
        original = Message("user123", "Test message with special chars: !@#$%^&*()")
        
        # Serialize to JSON
        json_str = original.to_json()
        
        # Deserialize back to message
        recovered = Message.from_json(json_str)
        
        # Verify all fields match
        self.assertEqual(original.sender_id, recovered.sender_id)
        self.assertEqual(original.content, recovered.content)
        self.assertEqual(original.message_type, recovered.message_type)
    
    def test_multiple_message_types(self):
        """Test handling of different message types."""
        message_types = ["text", "image", "file", "system", "error"]
        
        for msg_type in message_types:
            message = Message("user", f"Content for {msg_type}", msg_type)
            json_str = message.to_json()
            recovered = Message.from_json(json_str)
            
            self.assertEqual(recovered.message_type, msg_type)
    
    def test_concurrent_message_creation(self):
        """Test message creation under concurrent conditions."""
        import threading
        import time
        
        messages = []
        errors = []
        
        def create_messages(thread_id):
            try:
                for i in range(10):
                    message = Message(f"user_{thread_id}", f"Message {i}")
                    messages.append(message)
                    time.sleep(0.001)  # Small delay
            except Exception as e:
                errors.append(e)
        
        # Create multiple threads
        threads = []
        for i in range(5):
            thread = threading.Thread(target=create_messages, args=(i,))
            threads.append(thread)
            thread.start()
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        # Verify results
        self.assertEqual(len(errors), 0, f"Errors occurred: {errors}")
        self.assertEqual(len(messages), 50, "Expected 50 messages")


if __name__ == '__main__':
    # Configure test runner
    unittest.main(verbosity=2)
