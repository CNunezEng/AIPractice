"""
Entry point for running the chat client.
Follows AIinfo.txt standards for clear entry points and error handling.
"""

import asyncio
import argparse
import logging
import sys

from chat_client import create_interactive_client


async def main():
    """Main function to run the chat client."""
    # Set up argument parsing
    parser = argparse.ArgumentParser(description="Simple Chat Client")
    parser.add_argument("--host", default="localhost", help="Server host to connect to")
    parser.add_argument("--port", type=int, default=8765, help="Server port to connect to")
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
    
    # Create and run client
    client = create_interactive_client()
    
    try:
        logger.info(f"Connecting to chat server at {args.host}:{args.port}")
        await client.start_interactive_session(args.host, args.port)
    
    except KeyboardInterrupt:
        logger.info("Received interrupt signal, shutting down...")
    except Exception as e:
        logger.error(f"Client error: {e}")
        sys.exit(1)


if __name__ == "__main__":
    asyncio.run(main())
