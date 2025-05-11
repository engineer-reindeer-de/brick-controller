export function mapOptions(optionsArray = []) {
    const result = {};
    optionsArray.forEach(({ name, value }) => {
      result[name] = value;
    });
    return result;
  }