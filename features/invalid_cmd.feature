Feature: invalid commands

  Scenario: User attempts to add and delete a bookmark from the same command
    Given the default list file does not exist
    When I run wd with arguments "-a -r"
    Then the output should match:
    """
    """
    And the exit status should be 1
    And stderr should match:
    """
Parameter incompatible with other arguments: -r
    """
    And the default list file should not exist

  Scenario: User attempts filter the output, but does not specify a filter type
    Given the default list file does not exist
    # No parameter to -e argument means that filter type is lacking
    When I run wd with arguments "-d -e"
    Then the output should match:
    """
    """
    And the exit status should be 1
    And stderr should match:
    """
No parameter specified for argument: -e
    """
    And the default list file should not exist

  Scenario: User attempts filter the output, but specifies an invalid filter type
    Given the default list file does not exist
    When I run wd with arguments "-d -e q"
    Then the output should match:
    """
    """
    And the exit status should be 1
    And stderr should match:
    """
Parameter to argument not recognised: -e
    """
    And the default list file should not exist

