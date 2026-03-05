"""
Test runner for Python chat application.
Following AIinfo.txt standards for automated testing.
"""

import unittest
import sys
import os
import json
from datetime import datetime

# Add src to path for imports
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

from interfaces import Message


class TestRunner:
    """Custom test runner with detailed reporting."""
    
    def __init__(self):
        self.test_results = {}
        self.start_time = None
        self.end_time = None
    
    def run_all_tests(self):
        """Run all test suites and generate report."""
        self.start_time = datetime.now()
        
        print("=" * 70)
        print("PYTHON CHAT APPLICATION - AUTOMATED TEST SUITE")
        print("=" * 70)
        print(f"Started at: {self.start_time}")
        print()
        
        # Discover and run test modules
        test_modules = [
            'test_interfaces',
            'test_server', 
            'test_client'
        ]
        
        total_tests = 0
        total_failures = 0
        total_errors = 0
        
        for module_name in test_modules:
            print(f"Running {module_name}...")
            module_result = self._run_test_module(module_name)
            
            self.test_results[module_name] = module_result
            total_tests += module_result['tests_run']
            total_failures += module_result['failures']
            total_errors += module_result['errors']
            
            print(f"  Tests: {module_result['tests_run']}, "
                  f"Failures: {module_result['failures']}, "
                  f"Errors: {module_result['errors']}")
            print()
        
        self.end_time = datetime.now()
        duration = self.end_time - self.start_time
        
        # Print summary
        self._print_summary(total_tests, total_failures, total_errors, duration)
        
        # Run integration tests with runtime inputs
        self._run_runtime_input_tests()
        
        # Generate test report
        self._generate_test_report(total_tests, total_failures, total_errors, duration)
        
        return total_failures == 0 and total_errors == 0
    
    def _run_test_module(self, module_name):
        """Run a specific test module."""
        loader = unittest.TestLoader()
        suite = loader.loadTestsFromName(module_name)
        runner = unittest.TextTestRunner(verbosity=2, stream=open(os.devnull, 'w'))
        result = runner.run(suite)
        
        return {
            'tests_run': result.testsRun,
            'failures': len(result.failures),
            'errors': len(result.errors),
            'success_rate': (result.testsRun - len(result.failures) - len(result.errors)) / result.testsRun * 100 if result.testsRun > 0 else 0
        }
    
    def _run_runtime_input_tests(self):
        """Run tests using runtime input file."""
        print("Running runtime input tests...")
        
        try:
            with open('test_inputs.json', 'r') as f:
                test_data = json.load(f)
            
            # Test standard cases
            self._test_message_cases(test_data['standard_cases'], "Standard Cases")
            
            # Test edge cases
            self._test_message_cases(test_data['edge_cases'], "Edge Cases")
            
            # Test wrong inputs
            self._test_message_cases(test_data['wrong_inputs'], "Wrong Inputs")
            
            # Test different message types
            self._test_message_cases(test_data['message_types'], "Message Types")
            
            # Test concurrent cases
            self._test_concurrent_cases(test_data['concurrent_test_cases'])
            
        except Exception as e:
            print(f"Error running runtime input tests: {e}")
    
    def _test_message_cases(self, cases, category_name):
        """Test a set of message cases."""
        print(f"  Testing {category_name}:")
        
        passed = 0
        total = len(cases)
        
        for i, case in enumerate(cases):
            try:
                message = Message(
                    case['sender_id'],
                    case['content'],
                    case.get('message_type', 'text')
                )
                
                is_valid = message.is_valid()
                expected = case['expected_valid']
                
                if is_valid == expected:
                    passed += 1
                else:
                    print(f"    FAILED: Case {i+1} - Expected {expected}, got {is_valid}")
                    print(f"      Sender: '{case['sender_id']}', Content: '{case['content'][:50]}...'")
                
            except Exception as e:
                print(f"    ERROR: Case {i+1} - {e}")
        
        print(f"    Passed: {passed}/{total}")
    
    def _test_concurrent_cases(self, concurrent_cases):
        """Test concurrent message creation."""
        print("  Testing Concurrent Cases:")
        
        import threading
        import time
        
        messages = []
        errors = []
        lock = threading.Lock()
        
        def create_messages(thread_data):
            try:
                thread_messages = []
                for msg_data in thread_data['messages']:
                    message = Message(
                        msg_data['sender_id'],
                        msg_data['content']
                    )
                    thread_messages.append(message)
                    time.sleep(0.001)  # Small delay
                
                with lock:
                    messages.extend(thread_messages)
            except Exception as e:
                with lock:
                    errors.append(e)
        
        # Create threads
        threads = []
        for thread_data in concurrent_cases:
            thread = threading.Thread(target=create_messages, args=(thread_data,))
            threads.append(thread)
            thread.start()
        
        # Wait for completion
        for thread in threads:
            thread.join()
        
        expected_messages = sum(len(case['messages']) for case in concurrent_cases)
        
        print(f"    Expected: {expected_messages}, Created: {len(messages)}, Errors: {len(errors)}")
    
    def _print_summary(self, total_tests, total_failures, total_errors, duration):
        """Print test summary."""
        print("=" * 70)
        print("TEST SUMMARY")
        print("=" * 70)
        print(f"Total Tests: {total_tests}")
        print(f"Passed: {total_tests - total_failures - total_errors}")
        print(f"Failures: {total_failures}")
        print(f"Errors: {total_errors}")
        print(f"Success Rate: {((total_tests - total_failures - total_errors) / total_tests * 100):.1f}%" if total_tests > 0 else "N/A")
        print(f"Duration: {duration}")
        print(f"Completed at: {self.end_time}")
        print()
    
    def _generate_test_report(self, total_tests, total_failures, total_errors, duration):
        """Generate detailed test report."""
        report = {
            'test_run': {
                'start_time': self.start_time.isoformat(),
                'end_time': self.end_time.isoformat(),
                'duration_seconds': duration.total_seconds(),
                'total_tests': total_tests,
                'passed': total_tests - total_failures - total_errors,
                'failures': total_failures,
                'errors': total_errors,
                'success_rate': ((total_tests - total_failures - total_errors) / total_tests * 100) if total_tests > 0 else 0
            },
            'module_results': self.test_results,
            'environment': {
                'python_version': sys.version,
                'platform': sys.platform
            }
        }
        
        # Save report
        report_file = f"test_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"Detailed test report saved to: {report_file}")


def main():
    """Main test runner function."""
    runner = TestRunner()
    success = runner.run_all_tests()
    
    if success:
        print("✅ All tests passed!")
        return 0
    else:
        print("❌ Some tests failed!")
        return 1


if __name__ == '__main__':
    sys.exit(main())
