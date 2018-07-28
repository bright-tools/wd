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

