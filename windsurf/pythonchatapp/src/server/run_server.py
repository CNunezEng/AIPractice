"""
Entry point for running the chat server.
Follows AIinfo.txt standards for clear entry points and error handling.
"""

import asyncio
import argparse
import logging
import sys

from chat_server import create_chat_server


async def main():
    """Main function to run the chat server."""
    # Set up argument parsing
    parser = argparse.ArgumentParser(description="Simple Chat Server")
    parser.add_argument("--host", default="localhost", help="Host to bind to")
    parser.add_argument("--port", type=int, default=8765, help="Port to bind to")
    parser.add_argument("--log-level", default="INFO", 
                       choices=["DEBUG", "INFO", "WARNING", "ERROR"],
                       help="Logging level")
    
    args = parser.parse_args()
    
    # Configure logging
    logging.basicConfig(
        level=getattr(logging, args.log_level),
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    logger = logging.getLogger(__name__)
    
    # Create and start server
    server = create_chat_server()
    
    try:
        logger.info(f"Starting chat server on {args.host}:{args.port}")
        await server.start_server(args.host, args.port)
        
        # Keep server running
        logger.info("Server running. Press Ctrl+C to stop.")
        while server.is_running:
            await asyncio.sleep(1)
    
    except KeyboardInterrupt:
        logger.info("Received interrupt signal, shutting down...")
    except Exception as e:
        logger.error(f"Server error: {e}")
        sys.exit(1)
    finally:
        await server.stop_server()
        logger.info("Server stopped.")


if __name__ == "__main__":
    asyncio.run(main())
