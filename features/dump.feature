Feature: dump

  Scenario: User dumps the bookmarks when the list file does not exist
    Given the default list file does not exist
    When I run wd with arguments "-d"
    Then the output should match:
    """
Dirlist has 0 entries of 100 used
    """
    And the exit status should be 0
    And stderr should match:
    """
Warning: Unable to load list file '(.+?)wb_list'\r*
Creating empty list
    """
    And the default list file should not exist

  # Ruby's chmod is a NOP in Windows
  @notwindows
  Scenario: User dumps the bookmarks when the list file has unfriendly permissions
    Given the default list file is empty
    Given the default list file is not readable
    When I run wd with arguments "-d"
    Then the output should match:
    """
Dirlist has 0 entries of 100 used
    """
    And the exit status should be 0
    And stderr should match:
    """
Warning: Unable to load list file '(.+?)wb_list'\r*
Creating empty list
    """

  Scenario Outline: User dumps a bookmark file with a single entry
    Given the default list file exists
    And the default list file contains a shortcut to '<dir>' named "<bookmark>" with timestamp "2013/12/04 18:16:43"
    When I run wd with arguments "-d"
    Then the output should contain "Dirlist has 1 entries of 100 used"
    And the output should contain a dumped shortcut to '<dir>' numbered 0 named "<bookmark>" with timestamp "12/04/13 18:16:43 Eur"
    And the exit status should be 0
    And stderr should match:
    """
    """
    And the default list file should exist

    @notwindows
    Examples:
      | dir  | bookmark |
      | /    | root     |
      | /usr | usr      |
    @onlywindows
    Examples:
      | dir         | bookmark |
      | c:          | see      |
      | c:\         | see      |
      | c:\Windows  | win      |

